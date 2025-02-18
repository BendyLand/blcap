# Bland Caption Tool

A simple CLI tool to automatically add captions to videos!

## Usage 

The default usage of this tool takes the name of a video file as an argument. It embeds soft captions into the video via ffmpeg and saves it to `<filename>_w_captions.mp4`:

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
   - Hardware accelaration is available on MacOS, which greatly increases efficiency.
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
```
```bash
blcap example.mp4 --combine transcript.srt
# Outputs: example_w_captions.mp4
# exmple_w_captions.mp4 will have the same soft captions as the default, but with the contents of transcript.srt.
```
```bash
blcap example.mp4 --combine transcript.srt --burn
# Outputs: example_w_captions.mp4
# exmple_w_captions.mp4 will have the same burned in captions as the --burn option, but with the contents of transcript.srt.
```
