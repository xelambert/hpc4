#include <iostream>
#include <mpi.h>

// Check if a number is prime
bool isPrime(int number)
{
    if (number <= 1)
        return false;

    for (int i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
            return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    int rank, size, n;
    int tag = 0;
    int nextProcess, prevProcess;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        std::cout << "Enter number to factorize:";
        std::cin >> n;
    }

    // Broadcast n value to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the ranks of the next and previous processes in the ring
    prevProcess = (rank - 1 + size) % size;
    nextProcess = (rank + 1) % size;

    int prime = 2;
    while (prime <= n)
    {
        if (isPrime(prime))
        {
            if (n % prime == 0)
            {
                std::cout << "Process " << rank << ": Found prime factor " << prime << std::endl;
                n /= prime;
            }
        }

        // Send the updated n value to the next process
        MPI_Send(&n, 1, MPI_INT, nextProcess, tag, MPI_COMM_WORLD);

        // Get the updated n value from the previous process
        MPI_Recv(&n, 1, MPI_INT, prevProcess, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        prime++;
    }

    MPI_Finalize();
    return 0;
}
