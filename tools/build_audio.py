import os
import os.path
import shutil
import json
import helpers
import time
import build
stats_start = time.time()

print("--------------------------------------------------------------------------------")
print("pmtech audio compression and conversion ----------------------------------------")
print("--------------------------------------------------------------------------------")

config = open("build_config.json")
build_config = json.loads(config.read())

pmtech_dir = build.correct_path(build_config["pmtech_dir"])

platform_name = build.get_platform_name()
print(platform_name)

audio_dir = os.path.join(os.getcwd(), "assets", "audio")
build_dir = os.path.join(os.getcwd(), "bin", platform_name, "data", "audio")
bin_dir = os.path.join(os.getcwd(), "bin", platform_name)

# make directories
if not os.path.exists(build_dir):
    os.makedirs(build_dir)

dll = "fmod64.dll"
if platform_name == "osx":
    dll = "libfmod.dylib"
elif platform_name == "linux":
    dll = "libfmod.so"

# copy fmod dll / dylib
print("copying dynamic library to binary dir")

src_file = os.path.join(pmtech_dir, "pen", "third_party", "fmod", "lib", platform_name, dll)
shutil.copy(src_file, bin_dir)

# copy audio files
if os.path.exists(audio_dir):
    print("copying audio to data dir")
    for f in os.listdir(audio_dir):
        print(src_file)
        src_file = os.path.join(audio_dir,f)
        shutil.copy(src_file, build_dir)

stats_end = time.time()
millis = int((stats_end - stats_start) * 1000)
print("Done (" + str(millis) + "ms)")