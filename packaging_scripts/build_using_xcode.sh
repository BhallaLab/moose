#!/bin/bash
(
    mkdir -p  _build_macosx_
    cd _build_macosx_
    PROJECT=Moose.xcodeproj
    cmake -G "Xcode" ..
    xcodebuild -list
    xcodebuild clean -project  $PROJECT -configuration Release -alltargets
    xcodebuild archive -project $PROJECT -configuration Release \
        -scheme moose.bin  \
        -archivePath Moose.xcarhive
    xcodebuild -exportArchive -archivePath Moose.xcarhive -exportPath Moose \
        -exportFormat pkg 
)
