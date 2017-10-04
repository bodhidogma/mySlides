# MySlides Screensaver

openGL slideshow for your photo collection

## Based on:

 - NeHeGL
 - Really Slick Screen Savers

## Leveraging :

 - [FreeImage](http://freeimage.sourceforge.net/) image parsing library
 - [sqlite3](https://sqlite.org/download.html) local database v3.20.1

### Supported environments:
 * Windows 32 / 64
   * MS Visual Studio 6 
   * MS Visual Studio 10 

### Build tools:
 * [ResEdit](https://github.com/ty733420/windows-utils/blob/master/ResEdit-x64.7z)

### Cmd Line
 * Windows:

   ```
   ScreenSaver           - Show the Settings dialog box.
   ScreenSaver /c        - Show the Settings dialog box, modal to the foreground window.
   ScreenSaver /p <HWND> - Preview Screen Saver as child of window <HWND>.
   ScreenSaver /s        - Run the Screen Saver. 
   ```
