#include "Matrix.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

Matrix globalMatrixA(2, 2);
Matrix globalMatrixB(2, 2);

// Authentication token (read from auth.token file). If empty, auth disabled.
std::string expectedToken = "";

static inline void trimInPlace(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// Helper for case-insensitive header search
static size_t findHeader(const std::string &haystack,
                         const std::string &needle) {
  auto it = std::search(haystack.begin(), haystack.end(), needle.begin(),
                        needle.end(), [](char ch1, char ch2) {
                          return std::tolower(ch1) == std::tolower(ch2);
                        });
  return (it != haystack.end()) ? std::distance(haystack.begin(), it)
                                : std::string::npos;
}

std::string extractTokenFromRequest(const std::string &request,
                                    const std::string &body) {
  // Check header: X-API-Key (case-insensitive)
  std::string headKey = "X-API-Key:";
  size_t pos = findHeader(request, headKey);
  if (pos != std::string::npos) {
    size_t lineEnd = request.find("\r\n", pos);
    std::string val = request.substr(pos + headKey.length(),
                                     lineEnd - (pos + headKey.length()));
    trimInPlace(val);
    return val;
  }

  // Check header: Authorization: Bearer <token>
  std::string authKey = "Authorization: Bearer ";
  pos = findHeader(request, authKey);
  if (pos != std::string::npos) {
    size_t lineEnd = request.find("\r\n", pos);
    std::string val = request.substr(pos + authKey.length(),
                                     lineEnd - (pos + authKey.length()));
    trimInPlace(val);
    return val;
  }

  // Check body JSON: "token":"..."
  size_t tPos = body.find("\"token\"");
  if (tPos != std::string::npos) {
    size_t colon = body.find(":", tPos);
    size_t quote1 = body.find("\"", colon);
    size_t quote2 = body.find("\"", quote1 + 1);
    if (quote1 != std::string::npos && quote2 != std::string::npos) {
      std::string val = body.substr(quote1 + 1, quote2 - quote1 - 1);
      trimInPlace(val);
      return val;
    }
  }
  return "";
}

std::string jsonEscape(const std::string &str) {
  std::string result;
  for (char c : str) {
    switch (c) {
    case '"':
      result += "\\\"";
      break;
    case '\\':
      result += "\\\\";
      break;
    case '\n':
      result += "\\n";
      break;
    case '\r':
      result += "\\r";
      break;
    case '\t':
      result += "\\t";
      break;
    default:
      result += c;
    }
  }
  return result;
}

// Helper to send all data
static bool sendAll(SOCKET s, const char *data, int len) {
  int total = 0;
  while (total < len) {
    int n = send(s, data + total, len - total, 0);
    if (n <= 0)
      return false;
    total += n;
  }
  return true;
}

std::string toFraction(double val) {
  if (std::abs(val) < 1e-12)
    return "0";
  double rounded = std::round(val);
  if (std::abs(val - rounded) < 1e-11)
    return std::to_string((long long)rounded);

  // Quick fraction search
  int maxDenom = 200; // Reduced from 2000 for performance
  for (int d = 2; d <= maxDenom; ++d) {
    double num = val * d;
    double rnum = std::round(num);
    if (std::abs(num - rnum) < 1e-10) {
      return std::to_string((long long)rnum) + "/" + std::to_string(d);
    }
  }

  std::ostringstream oss;
  oss << std::setprecision(8)
      << val; // Reduced precision slightly for speed/scannability
  return oss.str();
}

std::string matrixToJSON(const Matrix &m) {
  std::ostringstream oss;
  oss << "\"";
  int rows = (int)m.data.rows();
  int cols = (int)m.data.cols();
  for (int i = 0; i < rows; ++i) {
    if (i > 0)
      oss << "\\n";
    for (int j = 0; j < cols; ++j) {
      if (j > 0)
        oss << " ";
      oss << (rows * cols > 2500 ? std::to_string(m.data(i, j))
                                 : toFraction(m.data(i, j)));
    }
  }
  oss << "\"";
  return oss.str();
}

std::string createSuccessResponse(const std::string &title,
                                  const std::string &result) {
  return std::string("{\"success\": true, \"title\": \"") + jsonEscape(title) +
         "\", \"result\": " + result +
         ", \"message\": \"Operation completed successfully\"}";
}

std::string createErrorResponse(const std::string &error) {
  return std::string("{\"success\": false, \"error\": \"") + jsonEscape(error) +
         "\"}";
}

std::string parseMatrixFromJSON(const std::string &json,
                                const std::string &matrixName) {
  size_t keyPos = json.find("\"" + matrixName + "\"");
  if (keyPos == std::string::npos)
    return "";

  size_t start = json.find("[", keyPos);
  if (start == std::string::npos)
    return "";

  int bracketCount = 0;
  size_t i = start;
  for (; i < json.length(); ++i) {
    if (json[i] == '[')
      bracketCount++;
    else if (json[i] == ']')
      bracketCount--;

    if (bracketCount == 0)
      break;
  }

  if (bracketCount == 0 && i < json.length()) {
    return json.substr(start, i - start + 1);
  }
  return "";
}

std::vector<std::vector<double>> parseMatrixString(const std::string &s) {
  std::vector<std::vector<double>> rows;
  if (s.empty())
    return rows;

  size_t n = s.size();
  size_t i = 0;
  // Find first outer bracket
  size_t outer = s.find("[[");
  if (outer == std::string::npos)
    outer = s.find("["); // Fallback
  if (outer == std::string::npos)
    return rows;

  i = outer + 1;
  while (i < n) {
    // Find start of row
    size_t startRow = s.find("[", i);
    if (startRow == std::string::npos)
      break;

    // Find end of row
    size_t endRow = s.find("]", startRow);
    if (endRow == std::string::npos)
      break;

    std::string rowContent = s.substr(startRow + 1, endRow - startRow - 1);
    std::vector<double> vals;
    std::stringstream ss(rowContent);
    std::string token;
    while (std::getline(ss, token, ',')) {
      try {
        size_t first = token.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
          continue;
        size_t last = token.find_last_not_of(" \t\n\r");
        vals.push_back(std::stod(token.substr(first, last - first + 1)));
      } catch (...) {
      }
    }
    if (!vals.empty())
      rows.push_back(vals);
    i = endRow + 1;

    // If we see another ] without a [ first, it's the end of outer
    size_t nextOpen = s.find("[", i);
    size_t nextClose = s.find("]", i);
    if (nextClose != std::string::npos &&
        (nextOpen == std::string::npos || nextClose < nextOpen)) {
      break; // End of matrix
    }
  }
  return rows;
}

void updateGlobalMatrices(const std::string &requestBody) {
  try {
    std::string matrixAStr = parseMatrixFromJSON(requestBody, "matrixA");
    if (!matrixAStr.empty()) {
      auto dataA = parseMatrixString(matrixAStr);
      if (!dataA.empty()) {
        globalMatrixA = Matrix(dataA);
      }
    }

    std::string matrixBStr = parseMatrixFromJSON(requestBody, "matrixB");
    if (!matrixBStr.empty()) {
      auto dataB = parseMatrixString(matrixBStr);
      if (!dataB.empty()) {
        globalMatrixB = Matrix(dataB);
      }
    }
  } catch (...) {
  }
}

std::string handleOperation(const std::string &operation,
                            const std::string &target = "A") {
  try {
    // Choose which matrix to operate on for single-matrix ops
    Matrix &M = (target == "B") ? globalMatrixB : globalMatrixA;

    if (operation == "addition") {
      Matrix result = globalMatrixA + globalMatrixB;
      return createSuccessResponse("Addition (A + B)", matrixToJSON(result));
    } else if (operation == "subtraction") {
      Matrix result = globalMatrixA - globalMatrixB;
      return createSuccessResponse("Subtraction (A - B)", matrixToJSON(result));
    } else if (operation == "multiplication") {
      Matrix result = globalMatrixA * globalMatrixB;
      return createSuccessResponse("Multiplication (A * B)",
                                   matrixToJSON(result));
    } else if (operation == "transpose") {
      Matrix result = M.transpose();
      return createSuccessResponse(std::string("Transpose (") + target + ")",
                                   matrixToJSON(result));
    } else if (operation == "rref") {
      Matrix result = M.rref();
      return createSuccessResponse(std::string("RREF (") + target + ")",
                                   matrixToJSON(result));
    } else if (operation == "ref") {
      Matrix result = M.ref();
      return createSuccessResponse(std::string("REF (") + target + ")",
                                   matrixToJSON(result));
    } else if (operation == "determinant") {
      double det = M.determinant();
      std::ostringstream ossd;
      ossd << std::fixed << std::setprecision(2) << det;
      return createSuccessResponse(std::string("Determinant (") + target + ")",
                                   std::string("{\"value\": ") + ossd.str() +
                                       "}");
    } else if (operation == "trace") {
      if (M.data.rows() != M.data.cols()) {
        return createErrorResponse("Trace only defined for square matrices");
      }
      double tr = 0.0;
      for (int i = 0; i < M.data.rows(); ++i)
        tr += M.data(i, i);
      std::ostringstream osst;
      osst << std::fixed << std::setprecision(2) << tr;
      return createSuccessResponse(std::string("Trace (") + target + ")",
                                   std::string("{\"value\": ") + osst.str() +
                                       "}");
    } else if (operation == "rank") {
      int rank = M.rank();
      return createSuccessResponse(std::string("Rank (") + target + ")",
                                   std::string("{\"value\": ") +
                                       std::to_string(rank) + "}");
    } else if (operation == "inverse") {
      Matrix result = M.inverse();
      return createSuccessResponse(std::string("Inverse (") + target + ")",
                                   matrixToJSON(result));
    } else if (operation == "independence") {
      bool indep = M.isLinearlyIndependent();
      return createSuccessResponse(
          std::string("Linear Independence (") + target + ")",
          std::string("{\"value\": ") + (indep ? "true" : "false") + "}");
    } else if (operation == "basis") {
      Matrix result = M.basis();
      std::ostringstream oss;
      oss << "[";
      for (size_t j = 0; j < result.data.cols(); ++j) {
        if (j > 0)
          oss << ", ";
        oss << "[";
        for (size_t i = 0; i < result.data.rows(); ++i) {
          if (i > 0)
            oss << ", ";
          oss << "\"" << toFraction(result.data(i, j)) << "\"";
        }
        oss << "]";
      }
      oss << "]";
      return createSuccessResponse(std::string("Basis (") + target + ")",
                                   oss.str());
    } else if (operation == "eigenvalues") {
      auto eigvals = M.eigenvalues();
      std::ostringstream oss;
      oss << "[";
      for (size_t i = 0; i < eigvals.size(); ++i) {
        if (i > 0)
          oss << ", ";
        oss << "\"" << toFraction(eigvals[i]) << "\"";
      }
      oss << "]";
      return createSuccessResponse(std::string("Eigenvalues (") + target + ")",
                                   oss.str());
    } else if (operation == "eigenvectors") {
      auto eigvecs = M.eigenvectors();
      std::ostringstream oss;
      oss << "[";
      for (size_t i = 0; i < eigvecs.size(); ++i) {
        if (i > 0)
          oss << ", ";
        oss << "[";
        for (size_t j = 0; j < eigvecs[i].size(); ++j) {
          if (j > 0)
            oss << ", ";
          oss << "\"" << toFraction(eigvecs[i][j]) << "\"";
        }
        oss << "]";
      }
      oss << "]";
      return createSuccessResponse(std::string("Eigenvectors (") + target + ")",
                                   oss.str());
    } else if (operation == "orthogonal") {
      bool ortho = M.isOrthogonal();
      bool orthonorm = M.isOrthonormal();
      std::string msg =
          "Orthogonal: " + std::string(ortho ? "Yes" : "No") +
          ", Orthonormal: " + std::string(orthonorm ? "Yes" : "No");
      return createSuccessResponse(std::string("Orthogonal/Orthonormal (") +
                                       target + ")",
                                   std::string("\"") + msg + "\"");
    } else if (operation == "gramschmidt") {
      Matrix ortho = M.gramSchmidt();
      // Calculate orthonormal basis by normalizing columns of ortho
      Matrix orthonorm(ortho.data.rows(), ortho.data.cols());
      for (size_t j = 0; j < ortho.data.cols(); ++j) {
        double norm = 0;
        for (size_t i = 0; i < ortho.data.rows(); ++i)
          norm += ortho.data(i, j) * ortho.data(i, j);
        norm = std::sqrt(norm);
        for (size_t i = 0; i < ortho.data.rows(); ++i)
          orthonorm.data(i, j) =
              (norm > 1e-10) ? (ortho.data(i, j) / norm) : 0.0;
      }

      auto toVecList = [](const Matrix &m) {
        std::ostringstream oss;
        oss << "[";
        for (size_t j = 0; j < m.data.cols(); ++j) {
          if (j > 0)
            oss << ", ";
          oss << "[";
          for (size_t i = 0; i < m.data.rows(); ++i) {
            if (i > 0)
              oss << ", ";
            oss << "\"" << toFraction(m.data(i, j)) << "\"";
          }
          oss << "]";
        }
        oss << "]";
        return oss.str();
      };

      std::string json = "{ \"orthogonal\": " + toVecList(ortho) +
                         ", \"orthonormal\": " + toVecList(orthonorm) + " }";
      return createSuccessResponse(std::string("Gram-Schmidt (") + target + ")",
                                   json);
    } else if (operation == "diagonalize") {
      Matrix P(M.data.rows(), M.data.cols());
      Matrix D = M.diagonalize(P);
      return createSuccessResponse(
          std::string("Diagonalization (") + target + ")", matrixToJSON(D));
    } else if (operation == "svd") {
      auto [U, S, V] = M.svd();
      return createSuccessResponse(std::string("SVD (") + target + ")",
                                   matrixToJSON(S));
    } else if (operation == "solve_system") {
      // For solve, we use A as coefficients and B as constants
      Matrix result = globalMatrixA.solveSystem(globalMatrixB);
      return createSuccessResponse("System Solution (RREF Augmented)",
                                   matrixToJSON(result));
    } else if (operation == "solve_cramer") {
      Matrix result = globalMatrixA.solveCramer(globalMatrixB);
      return createSuccessResponse("Cramer's Rule Solution",
                                   matrixToJSON(result));
    } else {
      return createErrorResponse("Unknown operation: " + operation);
    }
  } catch (const std::exception &e) {
    return createErrorResponse(std::string(e.what()));
  }
}

std::string
generateHTTPResponseWithStatus(const std::string &body, int code = 200,
                               const std::string &statusText = "OK") {
  std::ostringstream oss;
  oss << "HTTP/1.1 " << code << " " << statusText << "\r\n";
  oss << "Content-Type: application/json\r\n";
  oss << "Access-Control-Allow-Origin: *\r\n";
  oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
  oss << "Access-Control-Allow-Headers: Content-Type, X-API-Key, "
         "Authorization\r\n";
  oss << "Content-Length: " << (long)body.length() << "\r\n";
  oss << "Connection: close\r\n";
  oss << "\r\n";
  oss << body;
  return oss.str();
}

std::string generateHTMLResponse(const std::string &html) {
  std::string response;
  response += "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/html; charset=utf-8\r\n";
  response += "Content-Length: " + std::to_string((long)html.length()) + "\r\n";
  response += "Connection: close\r\n";
  response += "\r\n";
  response += html;
  return response;
}

std::string generate404Response() {
  std::string html = "<html><body><h1>404 Not Found</h1></body></html>";
  std::string response;
  response += "HTTP/1.1 404 Not Found\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Content-Length: " + std::to_string((long)html.length()) + "\r\n";
  response += "Connection: close\r\n";
  response += "\r\n";
  response += html;
  return response;
}

void handleClient(SOCKET clientSocket) {
  std::string fullRequest;
  char buffer[16384];
  bool headersRead = false;
  size_t contentLength = 0;
  size_t expectedTotal = 0;
  size_t bodyStartPos = 0;

  // 1. Read headers first
  while (!headersRead) {
    int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (recvSize <= 0) {
      closesocket(clientSocket);
      return;
    }
    fullRequest.append(buffer, recvSize);

    size_t pos = fullRequest.find("\r\n\r\n");
    if (pos != std::string::npos) {
      headersRead = true;
      bodyStartPos = pos + 4;

      size_t clPos = findHeader(fullRequest, "Content-Length:");
      if (clPos != std::string::npos) {
        size_t lineEnd = fullRequest.find("\r\n", clPos);
        std::string line = fullRequest.substr(clPos, lineEnd - clPos);
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
          std::string val = line.substr(colon + 1);
          try {
            contentLength = std::stoul(val);
          } catch (...) {
            contentLength = 0;
          }
        }
      }
      expectedTotal = bodyStartPos + contentLength;
    }
    // Limit headers to 64KB, Body to 10MB
    if (fullRequest.length() > 65536 && !headersRead) {
      closesocket(clientSocket);
      return;
    }
  }

  if (expectedTotal > 100 * 1024 * 1024 + bodyStartPos) {
    closesocket(clientSocket);
    return;
  }

  // 2. Read remaining body if needed
  while (fullRequest.length() < expectedTotal) {
    int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (recvSize <= 0)
      break;
    fullRequest.append(buffer, recvSize);
  }

  if (fullRequest.length() >= bodyStartPos) {
    std::string request = fullRequest.substr(0, bodyStartPos);
    std::string body = fullRequest.substr(bodyStartPos);
    std::string response;

    // Handle OPTIONS requests (CORS preflight)
    if (request.find("OPTIONS") == 0) {
      response = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: Content-Type, X-API-Key, "
                 "Authorization\r\n"
                 "Content-Length: 0\r\nConnection: close\r\n\r\n";
      sendAll(clientSocket, response.c_str(), (int)response.length());
    } else if (request.find("POST /api/operation") != std::string::npos) {
      std::string receivedToken = extractTokenFromRequest(request, body);
      if (!expectedToken.empty() && receivedToken != expectedToken) {
        std::string err = createErrorResponse("Unauthorized: invalid token");
        std::string resp =
            generateHTTPResponseWithStatus(err, 401, "Unauthorized");
        sendAll(clientSocket, resp.c_str(), (int)resp.length());
      } else {
        updateGlobalMatrices(body);
        std::string op;
        size_t opPos = body.find("\"operation\"");
        if (opPos != std::string::npos) {
          size_t colonPos = body.find(":", opPos);
          size_t quote1 = body.find("\"", colonPos);
          size_t quote2 = body.find("\"", quote1 + 1);
          if (quote1 != std::string::npos && quote2 != std::string::npos) {
            op = body.substr(quote1 + 1, quote2 - quote1 - 1);
          }
        }
        std::string target = "A";
        size_t tPos = body.find("\"target\"");
        if (tPos != std::string::npos) {
          size_t colonPos = body.find(":", tPos);
          size_t quote1 = body.find("\"", colonPos);
          size_t quote2 = body.find("\"", quote1 + 1);
          if (quote1 != std::string::npos && quote2 != std::string::npos) {
            target = body.substr(quote1 + 1, quote2 - quote1 - 1);
          }
        }
        std::string jsonResponse = handleOperation(op, target);
        response = generateHTTPResponseWithStatus(jsonResponse);
        sendAll(clientSocket, response.c_str(), (int)response.length());
      }
    } else if (request.find("GET /") == 0) {
      std::ifstream file("index.html", std::ios::binary);
      if (file.good()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        response = generateHTMLResponse(buffer.str());
      } else {
        response = generate404Response();
      }
      sendAll(clientSocket, response.c_str(), (int)response.length());
    } else {
      response = generate404Response();
      sendAll(clientSocket, response.c_str(), (int)response.length());
    }
  }

  closesocket(clientSocket);
}

int main() {
  std::cout << "============================================\n";
  std::cout << "Linear Algebra GUI - HTTP Server v3 (Cross-Platform)\n";
  std::cout << "============================================\n\n";

#ifdef _WIN32
  std::cout << "[*] Initializing Winsock...\n";
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "[ERROR] WSAStartup failed\n";
    return 1;
  }
  std::cout << "[OK] Winsock initialized\n\n";
#endif

  std::cout << "[*] Creating socket...\n";
  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == INVALID_SOCKET) {
    std::cerr << "[ERROR] Socket creation failed\n";
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }
  std::cout << "[OK] Socket created\n\n";

  int port = 8080;
  const char *envPort = std::getenv("PORT");
  if (envPort) {
    try {
      port = std::stoi(envPort);
    } catch (...) {
      port = 8080;
    }
  }

  std::cout << "[*] Binding socket to port " << port << "...\n";
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);

  int reuseAddr = 1;
#ifdef _WIN32
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddr,
             sizeof(reuseAddr));
#else
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr,
             sizeof(reuseAddr));
#endif

  if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
      SOCKET_ERROR) {
    std::cerr << "[ERROR] Bind failed. Port 8080 may already be in use\n";
    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }
  std::cout << "[OK] Socket bound to 0.0.0.0:8080\n\n";

  std::cout << "[*] Starting to listen...\n";
  if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
    std::cerr << "[ERROR] Listen failed\n";
    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }
  std::cout << "[OK] Listening started\n\n";

  // Initialize default matrices
  std::vector<std::vector<double>> dataA = {{1, 2}, {3, 4}};
  std::vector<std::vector<double>> dataB = {{5, 6}, {7, 8}};
  globalMatrixA = Matrix(dataA);
  globalMatrixB = Matrix(dataB);

  // Load authentication token if present (auth.token)
  std::ifstream tf("auth.token");
  if (tf.good()) {
    std::string tok;
    std::getline(tf, tok);
    trimInPlace(tok);
    if (!tok.empty()) {
      expectedToken = tok;
      std::cout << "[SECURITY] auth.token found. API token ENABLED.\n";
    }
  }

  std::cout << "============================================\n";
  std::cout << "SERVER RUNNING SUCCESSFULLY!\n";
  std::cout << "============================================\n";
  std::cout << "[+] Server Address: http://127.0.0.1:8080\n";
  std::cout << "[+] Status: Ready for connections\n";
  std::cout << "============================================\n\n";

  while (true) {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);

    if (clientSocket != INVALID_SOCKET) {
      std::cout << "[+] Connection from " << inet_ntoa(clientAddr.sin_addr)
                << std::endl;
      std::thread(handleClient, clientSocket).detach();
    }
  }

  closesocket(serverSocket);
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
