#!/bin/bash

git pull

cd server/

npm install
pm2 restart photo --update-env || pm2 start npm --name "photo" -- run start

pm2 save

echo "Application 'photo' mise à jour et redémarrée avec succès."
