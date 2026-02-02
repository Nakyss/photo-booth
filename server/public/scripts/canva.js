import { canvas } from "./htmlElement.js";
import { getCaption } from "./inputs.js";
import { rooms } from "./room.js";

let originalImage = null;
const ctx = canvas.getContext('2d');

/**
 * Define the size of the canva based on the media dimensions,
 * tends to 1920x1080 while keeping the aspect ratio
 * @param {int} mediaW Width of the media
 * @param {int} mediaH Height of the media
 * @return {object} Object containing the width and height of the canva
 */
const setCanvaSize = (mediaW, mediaH) => {
    const maxW = 1920;
    const maxH = 1080;
    let width, height;
    if (mediaW / mediaH > maxW / maxH) {
        width = maxW;
        height = Math.round((mediaH * maxW) / mediaW);

    } else {
        width = Math.round((mediaW * maxH) / mediaH);
        height = maxH;
    }
    canvas.width = width;
    canvas.height = height;
    return { width: width, height: height }
};

/**
 * Display caption text on the canvas
 * @param {string} text 
 */
const drawCaptionOnCanvas = (text) => {
    if (!text) return;
    
    const width = canvas.width;
    const height = canvas.height;
    
    // Configuration du style du texte
    const fontSize = Math.floor(height / 12); // Taille proportionnelle à l'image
    ctx.font = `bold ${fontSize}px Arial`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';
    
    const padding = fontSize / 2 ;
    const maxWidth = width - (padding * 2);
    
    // Découper le texte en lignes si nécessaire
    const words = text.split(' ');
    const lines = [];
    let currentLine = words[0];
    
    for (let i = 1; i < words.length; i++) {
        const word = words[i];
        const testLine = currentLine + ' ' + word;
        const metrics = ctx.measureText(testLine);
        
        if (metrics.width > maxWidth) {
            lines.push(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    lines.push(currentLine);
    
    // Position de départ (en bas de l'image)
    const lineHeight = fontSize * 1.2;
    let y = height - padding;
    
    // Dessiner chaque ligne
    for (let i = lines.length - 1; i >= 0; i--) {
        const x = width / 2;
        
        // Contour noir
        ctx.strokeStyle = 'black';
        ctx.lineWidth = 5;
        ctx.lineJoin = 'round';
        ctx.strokeText(lines[i], x, y);
        
        // Texte blanc
        ctx.fillStyle = 'white';
        ctx.fillText(lines[i], x, y);
        
        y -= lineHeight;
    }
}


/**
 * Generate a Blob from the canvas and upload it to the server
 * @return {Promise} Resolves with server response or rejects with error
 */
const uploadCapturedPhoto = () => {
    return new Promise((resolve, reject) => {
        // Convertir le canvas haute résolution en blob
        canvas.toBlob(async (blob) => {
            if (!blob) return reject(new Error('Impossible de créer le blob'));

            const form = new FormData();
            form.append('image', blob, `photo-${Date.now()}.png`);
            form.append('message', getCaption());
            form.append('rooms', JSON.stringify(rooms));
            
            try {
                const res = await fetch('/upload', {
                    method: 'POST',
                    body: form
                });
                if (!res.ok) {
                    const err = await res.text();
                    return reject(new Error(err || 'Erreur lors de l\'upload'));
                }
                const json = await res.json();
                resolve(json);
            } catch (e) {
                reject(e);
            }
        }, 'image/png');
    });
}

const setOriginalImage = (imageData) => {
    originalImage = imageData;
};

export { 
    ctx,
    originalImage,
    setOriginalImage,
    setCanvaSize, 
    drawCaptionOnCanvas,
    uploadCapturedPhoto
};