import sys
import subprocess
import time


if len(sys.argv) != 2:
    print("Usage: python script.py <input_file>")
    sys.exit(1)

input_file = sys.argv[1]

start_time = time.time()
try:
    # Run the program with the input file
    with open(input_file) as inpf:
        subprocess.run(['./a.out'], check=True, stdin=inpf)
except subprocess.CalledProcessError as e:
    print(f"Error: {e}")
    sys.exit(1)

end_time = time.time()
execution_time = end_time - start_time

print(f"Execution time: {execution_time:.2f} seconds")