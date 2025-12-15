import { captionInput } from "./htmlElement.js";


/**
 * Get the text from the caption input
 * @returns { string } content of the caption input
 */
const getCaption = () => {
    return captionInput.value.trim();
}

export { getCaption };