# WalOS

It's a small x86 OS.


----

#####Reference
[Write your own OS with Free and Open source software(楊文博)](http://share.solrex.cn/WriteOS/)

[Orange'S 一個作業系統的實現(于淵)](http://forrestyu.net/)

[程式設計師的自我修養：連結、載入、程式庫](http://www.books.com.tw/products/0010456858)

[Intel® 64 and IA-32 Architectures Software Developer’s Manual](http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html)

[Executable and Linkable Format (ELF) document](http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf)

----
#####Current Flow(6/11)
```
            -------------------------------------------------------
BootSector  |       Boot to BootSector
            |
            |       Load loader
Loader      |       Jump to loader
            |       
            |       Load kernel
Kernel      |       Jump to kernel
            |   
            |       Initial and switch to kernel GDT 
            |       
            |       Initial hardware interrupt controller(8259A)
            |       Initial software interrupt(system call)
            |       Initial IDT
            |       Initial TSS and LDT descriptor in GDT.
            |       
            |
            |
            |       Initial Process Table
            |       Initial Clock interrupt
            |       
            |       Process start...
            |       A, B and C start around
            |       In A process, I demo a simple system call to get current tick.
            -------------------------------------------------------
```



----

#####Memory Layout(5/17)
```
        -------------------------
        |                       |        
        |     Page Tables       |
0x101000|-----------------------|
        |        PDT            |
0x100000|-----------------------|
        |                       |
        |     Hardware          |        
        |        Reserved       |
        |                       |
        |-----------------------|        
        |        EBDA           |
        |-----------------------|
        |     loader.bin        |        
        |                       |
0x90000 |-----------------------|
        |     kernel.bin        |        
        |                       |
0x80000 |-----------------------|
        |     kernel            |        
        |           Entry       |
0x30000 |-----------------------|
        |                       |        
        |                       |
        |                       |        
        |                       |
        |                       |        
        |                       |
0x0     -------------------------
```

```
GNU GENERAL PUBLIC LICENSE
Copyright (C) 2015  WalonLi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
