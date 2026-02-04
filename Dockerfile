# Build stage
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy project files
COPY . .

# Compile the server
# Note: Matrix.cpp includes <Eigen/Dense>, so we point to the Eigen3 directory
RUN g++ -O3 -I./Eigen3 -pthread server.cpp Matrix.cpp -o server

# Run stage
FROM ubuntu:22.04

WORKDIR /app

# Copy only the compiled binary and the web interface
COPY --from=builder /app/server .
COPY --from=builder /app/index.html .

# Expose the server port
EXPOSE 8080

# Command to run the server
CMD ["./server"]
