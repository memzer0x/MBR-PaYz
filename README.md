# MBR-PaYz
MBR-PaYloadz Cross Compiler Master Boot Record Payload Generator for UselessDisk

*The project works both on linux and windows !*

The project is quite simple, it will first ask you for a title (60 characters maximum) and it will asks you for 5 lines of 80 characters each where you can write your messages that you want to overwrite the Master Boot Record with.

You can type **END** or **EMPTY**, to stop reading or to input a fully empty line respectively.

<p align=center>
  <img src="https://cdn.discordapp.com/attachments/894719060556804130/922666374608859156/unknown.png"/>
  <img src="https://cdn.discordapp.com/attachments/895523353316716605/922666980744499230/unknown.png"/>
</p>

You can compile and use the project the following way
```
git clone https://github.com/memzer0x/MBR-PaYz
cd MBR-PaYz
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd Release
.\MBR-PaYz
```

Thanks to the creators of the [fmt](https://github.com/fmtlib/fmt) for creating this awesome formatting library that has became part of C++ 20, has you may have noticed i put the fmt library in the project since there is no compilers that support the C++ 20 formatting library yet.
