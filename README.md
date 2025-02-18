# Bland Caption Tool

A simple CLI tool to automatically add captions to videos!

## Installation

This project has two major dependencies: whisper.cpp and FFmpeg. The steps to download both are below:

### Downloading Whisper

1) In the root directory of this project (blcap), clone the repository for `whisper.cpp`:
   - `git clone https://github.com/ggerganov/whisper.cpp.git`
2) Build whisper.
```bash
cd whisper.cpp
make # For Unix-like systems; Windows usage TBD
```
3) Download a Whisper model.
   - `bash ./models/download-ggml-model.sh base`
   - You can swap in `tiny`, `small`, or `medium` for `base`, depending on your preferences.
  
### Downloading FFmpeg

1) Install FFmpeg via your system's package manager.
```bash
# macOS
brew install ffmpeg
```
```bash
# Linux
# Debian
sudo apt update
sudo apt install ffmpeg
# Arch
sudo pacman -S ffmpeg
# Fedora
sudo dnf install 
```
```bash
# Windows
choco install ffmpeg
```
2) Verify installation.
   - `ffmpeg -version`
   - If it is still not found, manually add FFmpeg to your system's PATH.

## Usage 

The default usage of this tool takes the name of a video file as an argument. It generates a subtitle file and embeds soft captions into the video using FFmpeg.

**IMPORTANT:** Before running the binary, there are two variables in the main file that must be filled in with the absolute paths to the whisper binary and the expected transcript path. There are comments in the `run` function of `main.cpp` that point them out. I hide the paths in text files, but you may simply hardcode the paths you need.

```bash
# Default behavior:
blcap example.mp4
# Outputs: example_w_captions.mp4
# example_w_captions.mp4 will have soft subtitles, which can be turned on and off in most players. 
```

There are various flags which change the behavior of the tool. The following options are available:

 - `--srt-only`: This will generate a file called 'transcript.srt' which contains the written transcript of the video along with the timestamps. 
   - This file can be manually edited just as any other text file.
 - `--burn`: This will burn the captions directly into the video instead of embedding soft captions. 
   - This is much more computationally expensive, but will result in permanent captions that can be viewed on any player.
   - Hardware acceleration is available on macOS, which greatly increases efficiency.
 - `--combine <srt_file>`: This flag will attempt to read an existing srt file rather than transcribe one fresh from the video. The rest of the behavior remains the same.
   - This is useful if you want to generate captions in advance, correct any inaccuracies manually, then apply the corrected captions. 
   - This flag can also be combined with `--burn` for permanent captions.

## Examples

```bash
blcap example.mp4
# Outputs: example_w_captions.mp4
# example_w_captions.mp4 will have soft subtitles, which can be turned on and off in most players. 
```
```bash
blcap example.mp4 --burn
# Outputs: example_w_captions.mp4
# example_w_captions.mp4 will have permanent captions, as if they were pre-rendered into the video.
```
```bash
blcap example.mp4 --srt-only
# Outputs: transcript.srt
# transcript.srt is a text file; it contains a transcript of the video in srt format.
# Note: No new video is created with this option!
```
```bash
blcap example.mp4 --combine transcript.srt
# Outputs: example_w_captions.mp4
# example_w_captions.mp4 will have the same soft captions as the default, but with the contents of transcript.srt.
```
```bash
blcap example.mp4 --combine transcript.srt --burn
# Outputs: example_w_captions.mp4
# example_w_captions.mp4 will have the same burned in captions as the --burn option, but with the contents of transcript.srt.
```
