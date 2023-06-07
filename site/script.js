var message = "";

function addCharacter(character) {
    message += character;
    var resultElement = document.getElementById("result");
    result = message;

    resultElement.value = result;
}

function hideButtons() {
    var buttonContainer = document.getElementById("buttonContainer");
    buttonContainer.style.display = "none";
}



var submittedText = "";

function submitText() {
    var inputElement = document.getElementById("textInput");
    submittedText = inputElement.value;
    inputElement.value = ""; // Clear the input field
    text = "0x" + submittedText.toUpperCase() + " ";
    addCharacter(text);
}

function codeMessage() {
    var codingOption = document.getElementById("codingOptions").value;
    var resultElement = document.getElementById("result");
   // var in = document.getElementById("messageInput").value;
    var result;

    result = message;

    resultElement.value = result;
}

function copyMessage() {
    var resultElement = document.getElementById("result");
  
    // Select the text in the result textarea
    resultElement.select();
    resultElement.setSelectionRange(0, 99999); // For mobile devices
  
    // Copy the text to the clipboard
    document.execCommand("copy");
}
