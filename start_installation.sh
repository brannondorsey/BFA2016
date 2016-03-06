#!/bin/bash

echo "Waiting 10 seconds before executing script..."
sleep 10

# SCRIPT_NAME=$(basename $0)
DIR_NAME=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

POCSAG_RECORDING_WAV="$DIR_NAME/data/POCSAG_recording.wav"
POCSAG_RECORDING_CSV="$DIR_NAME/data/POCSAG_recording.csv"

# POCSAG audio recording
echo "Playing \"$POCSAG_RECORDING_WAV\" in the background."
sleep 21 && cat $POCSAG_RECORDING_WAV | aplay >> "$DIR_NAME/log/audio.log" 2>&1 &
AUDIO_PID=$!

# ESCPOS thermal printer
echo "Launching ESCPOSPrinterDaemon in the background."
$DIR_NAME/ESCPOSPrinterDaemon/bin/ESCPOSPrinterDaemon >> "$DIR_NAME/log/ESCPOSPrinterDaemon.log" 2>&1 &

# POCSAG pager text recording
echo "Launching holypager reading from file \"$POCSAG_RECORDING_CSV\"."
/usr/local/bin/node $DIR_NAME/holypager/holypager "--read=$POCSAG_RECORDING_CSV" --broadcast-freq=163.25e6 --bfa --anonymize --thermal --verbose | tee $DIR_NAME/log/holypager.log
