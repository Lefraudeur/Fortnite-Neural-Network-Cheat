# AI (neural-network external) Fortnite Cheat

### **What is this?**
This is a public release of a once-paid neural network aimbot for Fortnite: Battle Royale.

### **Is it undetected and up-to-date?**
As with any cheat, nothing is completely undetectable. **I do NOT recommend downloading the executable directly from the Releases page, as it has likely been detected already.** It is much safer to build the cheat yourself, which makes it unique and nearly undetectable.

### **How to build/use**
1. Download the source code using the **Download as ZIP** option or clone this repository using [git](https://git-scm.com/).

2. 
   - **Easy Method:** If you have Visual Studio 2022 (Community) installed, navigate to the `Project` directory and run `Build.bat`
   - **Normal Method:** Open the solution file (`Fortnite.sln`) in Visual Studio 2022 and build it normally.

3. At this point, if the build succeeds, your output files will be in the `Build` folder (next to the `Project` folder). You should see the files `<yourcheat>.exe`, `Network.cfg`, and `Network.weights`.

4. Download the Nvidia cuDNN and OpenCV dependencies bundle from [here](https://mega.nz/file/q4hk2STD#cGJ8IyHvlPCAAzm9x14C2Wtb-wDdnGaU1VTTvum1CeU). Extract this archive and place all of the DLLs into the same folder as your `<yourcheat>.exe` executable.

5. Run the `<yourcheat>.exe` executable. I shouldn't have to tell you this.

### **Q/A**
- Is this program a virus?
    - I don't care that much about your information or computer, so no. If you don't believe me, go through the source code and see for yourself.
- Is it up-to-date?
    - This cheat doesn't require updating since it doesn't interface directly with Fortnite. All the information is gathered from the screen instead of through detectable memory reads, and information is sent to Fortnite through mouse movements instead of memory writes.
- **What graphics cards does this work with?**
    - This cheat's version of [OpenCV](https://opencv.org/) was built for CUDA versions 6.1, 7.5, and 8.6. These vaguely correspond to the NVIDIA GeForce 10, 20, and 30-series releases. To see if your card is compatible with these versions, check [here](https://en.wikipedia.org/wiki/CUDA#GPUs_supported)
- Does this work on AMD graphics cards or other graphics processors?
    - Maybe. The compatibility is a bit unknown, but it has worked on some Radeon cards and even Intel integrated graphics, sometimes.