#include "Params.hpp"

void Params::setMethod(const string& method) {
  if (method == "p")
    this->method = EncryptionMetod::Pass;
  else if (method == "a")
    this->method = EncryptionMetod::Actions;
  else if (method == "b")
    this->method = EncryptionMetod::Both;
  else {
    cout << "Unknown method " << method << endl;
    exit(1);
  }
}

void Params::setDirection(const string& direction) {
  if (direction == "e")
    mode = true;
  else if (direction == "d")
    mode = false;
  else {
    cout << "Unknown direction " << direction << endl;
    exit(1);
  }
}

void Params::setHashAlg(const string& hashAlg) {
  if (hashAlg == "None")
    hAlg = HashAlgorithm::None;
  else if (hashAlg == "MD5")
    hAlg = HashAlgorithm::MD5;
  else if (hashAlg == "SHA256")
    hAlg = HashAlgorithm::SHA256;
  else if (hashAlg == "SHA512")
    hAlg = HashAlgorithm::SHA512;
  else {
    cout << "Unknown hash algorithm " << hashAlg << endl;
    exit(1);
  }
}

void Params::setBlockSize(const string& blockSize) {
  uint16_t powerOfTwo;
  try {
    powerOfTwo = stoul(blockSize);
    if ((1Ui64 << powerOfTwo) < thread::hardware_concurrency())
      throw "Small block size\n";
    this->blockSize = 1Ui64 << powerOfTwo;
  }
  catch (exception e) {
    cerr << "Entered not a number.";
    exit(1);
  }
  catch (const char* c) {
    cerr << c << endl;
    exit(1);
  }
}

void Params::setFilePath(const string& filePath) {
  if (filesystem::exists(filePath))
    path = filePath;
  else {
    cout << "File not exists!\n";
    exit(1);
  }
}

void Params::setActionsFilePath(const string& actionsFilePath) {
  if (filesystem::exists(actionsFilePath))
    actionPath = actionsFilePath;
  else {
    cout << "Actions file not exists!\n";
    exit(1);
  }
}
