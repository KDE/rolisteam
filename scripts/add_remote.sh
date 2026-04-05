#!/bin/sh


GIT_ACCOUNT=renaudg


echo "##################################"
echo "# Add remote to diceparser       #"
echo "##################################"
cd s3yyrc/libraries/diceparser 
git remote add perso git@invent.kde.org:$GIT_ACCOUNT/rolisteam-diceparser.git
cd -
