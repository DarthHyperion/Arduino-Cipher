
const String keys[] = {
  "TTZUZRZJHP",
  "SUENIFKGAT",
  "CEXGWKMCVQ",
  "SIACAKRFM",
  "CSMKSWSNLH",
  "IPIJWROJBB",
  "SZSGAMYJGJ",
  "BIHLBNUIJS",
  "UTFGSQAISS",
  "SSPXJXURJV",
  "XPDRYPOFAC",
  "DROIFANPZE",
  "DKTZXUQLAM",
  "UDHAWMAJTG",
  "HKTQMEPWXS",
  "IRUQAHZSBP",
  "ZHZAAGTMTN",
  "EHORTISSLQ",
  "NYZZQALWTP",
  "PEEGDJLQLQ",
  "UNQGHTXFSG",
  "NOBSTVFLAX",
  "GQJYEVUNCX",
  "UTJDNOCLSP",
  "XQNGBDOBEN",
  "FURHLIZSIN",
  "AAAPEIGRHI",
  "QWQFCCVFPV",
  "KZEFJHUJWB",
  "NDJNLHURJO"
};


char readyToSend[32] = {'\0'}; //global array, which is to be transmitted via RF24
char recieved[32] = {'\0'}; // global array, which is to be recieved via RF24
int keyRecieved = 0; //global key number, which is to be embedded as the last char recieved[]



String loadInput() { //this functions takes the input string and checks it's validity
  bool errorFlag = false;
  String(inputStr);
  do {
    errorFlag = false;

    while (Serial.available () == 0) {}
    inputStr = Serial.readString();

    inputStr.remove((inputStr.length() - 2), 2);
    int inputLength = inputStr.length();

    if (inputLength > 30) {
      Serial.println("Input longer than 30 chars.");
      errorFlag = true;
    }

  } while (errorFlag);

  return inputStr;
}
String cipherText(String str, const String key) //this function encrypts the message, and returns the encrypted version
{
  String cipher_text;
  for (int i = 0; i < str.length(); i++)
  {
    if (isWhitespace(str[i])) {
      cipher_text += ' ';

    }
    else {
      char x = ((int)(str[i] - 33) + (int)(key[i % key.length()] - 33)) % 94;

      x += 33;
      cipher_text += x;
    }
  }
  return cipher_text;
}

String originalText( String cipher_text, const String key) //this function decrypts the message, and returns the decrypted version
{
  String orig_text;

  for (int i = 0 ; i < cipher_text.length(); i++)
  {
    if (isWhitespace(cipher_text[i])) {
      orig_text += ' ';

    }
    else {
      // converting in range 0-94

      char x = ((cipher_text[i] - 33) - (key[i % key.length()] - 33) + 94) % 94;

      // convert into (ASCII)
      x += 33;
      orig_text += x;
    }
  }
  return orig_text;
}

void prepForTransmit(String input, int keyNumber) { //this function makes pads the string upto 31 with '0', then embeds key


  input.toCharArray(readyToSend, 32);
  int zeroesStart = input.length();


  char keyNumberAsChar = (char)(keyNumber + 65);

  readyToSend[30] = keyNumberAsChar;

  return;
}


String PrepForDecode() { ////this function makes extracts key, then removes padding
  keyRecieved = (recieved[30] - 65);

  String toEdit = String(recieved);
  toEdit.remove(30, 1);

  if (toEdit.indexOf('\0') != 31) {
    toEdit.remove(toEdit.indexOf('\0'), (30 - toEdit.indexOf('\0')));
  }

  else {

  }
  return toEdit;

}

void Transmit() {
  randomSeed(analogRead(A0)); //SEED RANDOM FUNCTION
  delay(100);
  String(text) = loadInput(); //GET INPUT
  Serial.print("You entered: ");
  Serial.println(text); //PRINT INPUT      <--------------------------------------
  int keynumber = random(25);                                     //              |
  String encrypted_message = cipherText( text, keys[keynumber]);  //ENCRYPT THIs _|
  Serial.print("Using key number ");
  Serial.println(keynumber);

  prepForTransmit(encrypted_message, keynumber); // PADDING AND ADD KEYNUMBER
  Serial.print("Encrypted Message is: ");

  Serial.println(readyToSend);


  for (int i = 0; i < 32; i++) {
    Serial1.write(readyToSend[i]);
  }

  Serial1.flush();
  Serial.println("Encrypted Message has been transmitted");

//  for (int i = 0; i < 32; i++) {
//    readyToSend[i] = '\0';
//  }

  
  Serial.println("________________________________________________________________________________");

  return;

}


void Recieve() {
  Serial.println("A message has been recieved!");
  delay(100);
  Serial1.readBytes(recieved, sizeof(recieved));

  Serial.print("Encrypted Message recieved: ");
  Serial.println(recieved);

  String toDecode = PrepForDecode(); // PREP RECIEVED STRING FOR DECODING (REMOVE PADDING, EXTRACT KEY)

  Serial.print("Deciphered Message:");
  Serial.println(originalText( toDecode , keys[keyRecieved])); // PRINT DECODED STRING
//  for (int i = 0; i < 32; i++) {
//    recieved[i] = '\0';
//  }
  Serial.println("________________________________________________________________________________");

  return;
}

void setup() {  // TEST CODE
  Serial.begin(9600);
  Serial1.begin(9600);

  Serial.println("To transmit a coded message, simply enter your message. Maximum length is 30 chars");
  Serial.println("If you recieve a message, you will be prompted immediately.");
  Serial.println("\nDO NOT ENTER A MESSAGE IF A MESSAGE IS BEING RECIEVED\n____________________________________________________");

}
void loop() { //main program

  while ((Serial.available () == 0) && (Serial1.available () == 0)) {}
  if (Serial1.available()) {
    Recieve();
  }
  else if (Serial.available()) {
    Transmit();
  }
}
