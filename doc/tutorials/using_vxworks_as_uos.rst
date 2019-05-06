.. _using_vxworks_as_uos:

Using VxWorks as User OS
########################

This tutorial describes how to run VxWorks as the User OS on the ACRN hypervisor. We are using Kaby Lake-based
NUC (model NUC7i5DNHE) in this tutorial. Other :ref:`ACRN supported platforms <hardware>` should work as well.

Introduction to VxWorks
***********************

VxWorks is a real-time operating system (RTOS) that can be used in embedded systems. It can be customized easily
and run on most processors designed for distributed computing.

Steps for Using VxWorks as User OS
**********************************

#. Build VxWorks

   Follow the `VxWorks Getting Started Guide <https://docs.windriver.com/bundle/vxworks_7_tutorial_kernel_application_workbench_sr0610/page/rbu1422461642318.html>`_
   to setup the VxWorks development environment and build the VxWorks Image.

   .. note::
      The following kernel configuration should be excluded:
        * INCLUDE_PC_CONSOLE
        * DRV_SIO_PCI_NS16550
        * SELECT_PC_CONSOLE_KBD

      The following kernel configuration should be included:
        * DRV_SIO_IA_NS16550
        * DRV_SIO_FDT_NS16550

      The following kernel configuration value should be set:
        * CONSOLE_BAUD_RATE = 115200
        * SYS_CLK_RATE_MAX = 1000

#. Build GRUB2 BootLoader Image

   We use grub-2.02 as the bootloader of VxWorks in this tutorial. You may gave other version of GRUB2 a try.
   We can download the grub-2.02 from `ftp://ftp.gnu.org/gnu/grub/grub-2.02.tar.gz`.

   .. code-block:: none

      $ zcat grub-2.02.tar.gz | tar xvf -
      $ cd grub-2.02
      $ ./autogen.sh
      $ ./configure --with-platform=efi --target=x86_64
      $ make
      $ ./grub-mkimage -p /EFI/BOOT -d ./grub-core/ -O x86_64-efi -o grub_x86_64.efi \
            boot efifwsetup efi_gop efinet efi_uga lsefimmap lsefi lsefisystab \
            exfat fat multiboot2 multiboot terminal part_msdos part_gpt normal \
            all_video aout configfile echo file fixvideo fshelp gfxterm gfxmenu \
            gfxterm_background gfxterm_menu legacycfg video_bochs video_cirrus \
            video_colors video_fb videoinfo video net tftp

   Then we will get ``grub_x86_64.efi`` in current directory.

#. Preparing the Boot Device

   .. code-block:: none

      $ dd if=/dev/zero of=VxWorks.img bs=1M count=35
      $ mkfs.vfat -F 32 VxWorks.img
      $ sudo mount `sudo losetup -f -P --show VxWorks.img` /mnt

    Create the following directories.

   .. code-block:: none

      $ sudo mkdir -p /mnt/efi/boot
      $ sudo mkdir -p /mnt/kernel

   Copy ``vxWorks`` and ``grub_x86_64.efi``.

   .. code-block:: none

      $ sudo cp vxWorks /mnt/kernel/
      $ sudo cp grub-2.02/grub_x86_64.efi /mnt/efi/boot/bootx64.efi

   Create ``/mnt/efi/boot/grub.cfg`` containing the following:

   .. code-block:: console

      set default=0
      set timeout=5

      menuentry "VxWorks Guest" {
          mutiboot /kernel/vxWorks
      }

   Unmount the loopback device:

   .. code-block:: none

      $ sudo umount /mnt

   You now have a virtual disk image with bootable VxWorks in ``VxWorks.img``. If the VxWorks build system
   is not the ACRN SOS, then you will need to transfer this image to ACRN SOS (via, e.g, a USB stick or network)

#. Follow :ref:`getting-started-apl-nuc` to boot "The ACRN Service OS".

#. Boot VxWorks as User OS.

   On the ACRN SOS, prepare a directory and populate it with VxWorks files.

   .. code-block:: none

      $ mkdir vxworks && cd vxworks
      $ cp /usr/share/acrn/samples/nuc/launch_vxworks.sh .
      $ cp /usr/share/acrn/bios/OVMF.fd .

   You will also need to copy the ``VxWorks.img`` created in the above section into directory ``vxworks``.

   Run the ``launch_vxworks.sh`` script to launch VxWorks as Uos.

   .. code-block:: none

      $sudo ./launch_vxworks.sh

   Then VxWorks will boot up automatically. You will see the prompt.

   .. code-block:: console

                     VxWorks 7 SMP 64-bit

      Copyright 1984-2019 Wind River Systems, Inc.

            Core Kernel version: 1.2.7.0
                     Build date: May  5 2019 21:40:28
                          Board: x86 Processor (ACPI_BOOT_OP) SMP/SMT
                      CPU Count: 1
                 OS Memory Size: ~1982MB
               ED&R Policy Mode: Permanently Deployed

      Adding 9315 symbols for standalone.

      ->
