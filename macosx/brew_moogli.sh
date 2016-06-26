#!/bin/bash
set -x

PATH=/usr/local/bin:$PATH

rm -f /Library/Caches/Homebrew/moogli*.tar.gz
rm -f /Library/Caches/Homebrew/moogli*.zip
cp ./moogli.rb /usr/local/Library/Formula/moogli.rb

brew -v install moogli --debug | tee _build_moogli.log

echo "Checking brew script for submission"
brew audit --strict moogli
brew -v test moogli
