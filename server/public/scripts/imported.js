import { setCanvaSize, ctx, drawCaptionOnCanvas, setOriginalImage } from "./canva.js";
import { hideVideo } from "./documentAction.js";
import { getCaption } from "./inputs.js";

/**
 * Display an imported image on the canvas
 * @param {string} dataURL 
 */
const displayImportedImage = dataURL => {
    const img = new Image();
    img.onload = () => {
        const canvaSize = setCanvaSize(img.width, img.height);

        ctx.drawImage(img, 0, 0, canvaSize.width, canvaSize.height);

        hideVideo();

        setOriginalImage(ctx.getImageData(0, 0, canvaSize.width, canvaSize.height));
        drawCaptionOnCanvas(getCaption());
    };
    img.src = dataURL;
}

/**
 * Handle file selection from the import input
 * @param {Event} event 
 */
const onFileSelected = event => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => displayImportedImage(e.target.result);
    reader.readAsDataURL(file);
}

/**
 * Handle paste event to import image from clipboard
 * @param {Event} event 
 */
const handlePasteImage = (event) => {
    const items = event.clipboardData?.items;
    if (!items) return;

    for (const item of items) {
        if (item.type.startsWith("image/")) {
            const file = item.getAsFile();
            const reader = new FileReader();

            reader.onload = (e) => {
                displayImportedImage(e.target.result);
            };

            reader.readAsDataURL(file);

            event.preventDefault();
            return;
        }
    }
};

/**
 * Handle drag over event, by modifying the appearance
 * @param {Event} event 
 */
const handleDragOver = (event) => {
    event.preventDefault();
    event.stopPropagation();
    document.body.classList.add('dragging');
};

/**
 * Handle drag leave event, restoring the appearance
 * @param {Event} event 
 */
const handleDragLeave = (event) => {
    event.preventDefault();
    event.stopPropagation();
    document.body.classList.remove('dragging');
};

/**
 * Handle drop event to import image from dragged file
 * @param {Event} event 
 */
const handleDrop = (event) => {
    event.preventDefault();
    event.stopPropagation();
    document.body.classList.remove('dragging');
    
    const files = event.dataTransfer?.files;
    if (!files || files.length === 0) return;
    
    const file = files[0];
    if (!file.type.startsWith("image/")) {
        alert("Veuillez déposer une image");
        return;
    }
    
    const reader = new FileReader();
    reader.onload = (e) => {
        displayImportedImage(e.target.result);
    };
    reader.readAsDataURL(file);
};

export { onFileSelected, handlePasteImage, handleDragOver, handleDragLeave, handleDrop };