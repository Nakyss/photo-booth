#!/bin/bash

# Vérifier si un screen "photo" existe
if screen -list | grep -q "\.photo\s"; then
    echo "Un screen 'photo' existe déjà. Arrêt en cours..."
    # Envoyer Ctrl+C au screen pour arrêter l'application
    screen -S photo -X stuff $'\003'
    # Attendre un peu que l'application s'arrête proprement
    sleep 2
    # Tuer le screen
    screen -S photo -X quit
    echo "Screen 'photo' arrêté."
fi

# Lancer le nouveau screen
echo "Lancement du nouveau screen..."
screen -dmS photo bash -c '
cd server/;
npm run start;
exec bash'

echo "Screen 'photo' lancé avec succès."