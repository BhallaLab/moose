This document describes how to create DMG package on MAC OSX.

1. Run `./build_dmg_image_using_brew.sh`

- This script create an empty dmg file.
- Mount it on /Volume/Moose_3.0.2
- Install brew in it with prefix /Volumes/Moose_3.0.2 and `brew install moose`
  with given prefix.
- It also adds an `moosegui` script which launches gui and also setup PYTHONPATH
  in ~/.bash_profile.

2. Run `./release_dmg_file.sh` 

- Copy the dmg created in step 1 and mount it.
- Remove unneccessay files `brew uninstall cmake gcc`. This saves us approx 400
  MB.
- Create another dmg file from folder and compress it.
- Compress this dmg file and test it on some MAC.
