from subprocess import Popen, PIPE


def call_cpp_program(instance, r_capt, r_comm):
    process = Popen(["./ProjetRO", instance, str(r_capt), str(r_comm)],
                    stdout=PIPE, cwd="../src/cmake-build-debug/")
    (output, err) = process.communicate()
    return int(output[-5:].lstrip())
