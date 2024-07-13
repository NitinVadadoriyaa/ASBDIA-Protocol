
# ASBDIA-PROTOCOL

ASBDIA(Algebraic Signatures-Based Data Integrity Auditing) is Challange response type protocol where cloude-user can periodicaly check integrity of they data which is stored on cloude (third party service provider) based on giving challange to cloud-service-provider. it check integrity of stored-data with help of cryptography hash-signature. .


## Technolgoy Stack
**Platform:** C++ && Crypto++ Library.

**Mode:** CLI(Command Line Interface).

**Utility:** Socket.


## Terminology
**Admin:** Client.

**Prover:** Cloud Service Provider.

**Challanger:** Truted Third Party.


## Working

- Admin,Prover & Challanger servers start.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/start.png)

- Challanger periodicaly give challange to Prover with randomly.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/start-1.png)

- Attaker Script run on Prover side for randomly change bits of Admin data.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/attaker.png)

- In Next few Challange, Challanger able to identify that data is change, so it immediatly report to Admin.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/errorReport.png)

## Design

- High Level View

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/HighView.png)

- First Admin generate Signature Of Data using this pattern & send it to Challanger, and then upload data to cloud.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/TagGen.png)

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/fileBlock.png)

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/fileBlock-2.png)

- When Prover get challange from challanger it start generating its proof of work. send it work to challange back.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/MueGen.png)

- Recieving proof of work from Prover, challanger start validating orignal-data using recieved information & Signature of data that it have.

![ScreenShot](https://github.com/NitinVadadoriyaa/ASBDIA-Protocol/blob/master/ScreenShort/verificationOnChallagerSIde.png)

## Made By
NitinVadadoriya.

## Location
IIITA Network Security Lab.