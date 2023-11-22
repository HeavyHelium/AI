import os
import subprocess
import time

# Specify the test-case directory
test_case_directory = "./cases"

# List all files in the test-case directory
files = os.listdir(test_case_directory)

# Filter for "in" files
in_files = [f for f in files if f.endswith(".in")]

for in_file in in_files:
    # Generate the corresponding "out" file name
    out_file = in_file.replace(".in", ".out")

    # Construct the full file paths
    in_file_path = os.path.join(test_case_directory, in_file)
    out_file_path = os.path.join(test_case_directory, out_file)

    # Run the program with the "in" file as input
    try:
        # Replace the following line with the command you want to run
        cmd = ["../a.out"]

        start_time = time.time()

        # Run the program and capture its output
        with open(in_file_path, "r") as input_file:
            output = subprocess.check_output(cmd, text=True, stdin=input_file, stderr=subprocess.STDOUT)

        end_time = time.time()
        execution_time = end_time - start_time

        # Read the content of the "out" file
        with open(out_file_path, "r") as out_file_content:
            expected_output = out_file_content.readlines()

        # Split lines, remove leading and trailing whitespace, and join them back
        output_lines = [line.strip() for line in output.strip().splitlines()]
        expected_lines = [line.strip() for line in expected_output]

        # Compare the program's output with the expected output
        if output_lines == expected_lines:
            print(f"Test case {in_file} passed", end="")
        else:
            print(f"Test case {in_file} failed")
        
        print(f" in{execution_time: .2f} seconds.")

    except Exception as e:
        print(f"Error running test case {in_file}: {e}")
