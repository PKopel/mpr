#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int msg_number = 2000;
const int max_msg_len = 2000;

void measure_time_Ibsend(int msg_size, int world_rank, char* message) {
    int sender = 0, receiver = (world_rank + 1) % 2;

    int buf_len;
    MPI_Pack_size(1, MPI_BYTE, MPI_COMM_WORLD, &buf_len);
    buf_len = buf_len * msg_size + MPI_BSEND_OVERHEAD;
    char* buf = (char*)malloc(buf_len);
    MPI_Buffer_attach(buf, buf_len);

    char msg_recv[max_msg_len];

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    int i = 0;
    for (; i < msg_number; i++) {
        if (world_rank == sender) {
            MPI_Request request;
            MPI_Ibsend(message, msg_size, MPI_BYTE, receiver, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(msg_recv, msg_size, MPI_BYTE, receiver, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        sender = (sender + 1) % 2;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    MPI_Buffer_detach(buf, &buf_len);

    free(buf);

    if (world_rank == 0) {
        double mean_time = (end - start) / ((double)msg_number);
        double Mbps = (msg_size * 8.0 / 1048576.0) / mean_time;
        printf("%d,%lf,%lf,ibsend\n", msg_size, mean_time, Mbps);
    }
}

void measure_time_Ssend(int msg_size, int world_rank, char* message) {
    int sender = 0, receiver = (world_rank + 1) % 2;

    char msg_recv[max_msg_len];

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    int i = 0;
    for (; i < msg_number; i++) {
        if (world_rank == sender) {
            MPI_Ssend(message, msg_size, MPI_BYTE, receiver, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(msg_recv, msg_size, MPI_BYTE, receiver, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        sender = (sender + 1) % 2;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (world_rank == 0) {
        double mean_time = (end - start) / ((double)msg_number);
        double Mbps = (msg_size * 8.0 / 1048576.0) / mean_time;
        printf("%d,%lf,%lf,ssend\n", msg_size, mean_time, Mbps);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming 2 processes for this task
    if (world_size != 2) {
        fprintf(stderr, "World size must be equal 2 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char message[max_msg_len];

    int i = 1;
    for (; i <= max_msg_len; i++) {
        measure_time_Ibsend(i, world_rank, message);

        measure_time_Ssend(i, world_rank, message);
    }

    MPI_Finalize();

    return 0;
}
