.. _using_zephyr_as_uos:

Using Zephyr as User OS
#######################

The tutorial describes the steps to run zephyr as User OS on ACRN hypervisor. We are using
Kaby Lake-based NUC model NUC7i5DNHE and you can try other platform ACRN supported as well.

Introduction to Zephyr
**********************

The Zephyr is a scalable real-time operating-system (RTOS) supporting multiple hardware architecture,
optimized for resource constrained devices, and built with safety and security in mind.

Steps for Using Zephyr as User OS
*********************************

#. Build Zephyr

   First of all, please follow the `Getting Started Guide
   <https://docs.zephyrproject.org/latest/getting_started/>`_ to
   setup Zephyr development environment. Then follow the `Board-Specfic Support
   <https://docs.zephyrproject.org/latest/boards/index.html>`_ to build the acrn supported zephyr image ``zephyr.elf``.
   
   .. note::
      When the `Getting Started Guide` for building acrn supported zephyr image got merged to `Zephyr Tree
      <https://github.com/zephyrproject-rtos/zephyr>`_, this section will be update to the exact reference.
  
#. Build grub2 boot loader image

   We can build grub2 bootloader for zephyr using ``boards/x86/common/scripts/build_grub.sh``
   which locate in `Zephyr Sourcecode <https://github.com/zephyrproject-rtos/zephyr>`_.

   .. code-block:: none

      $ ./boards/x86/common/scripts/build_grub.sh x86_64

   Find the binary at boards/x86/common/scripts/grub/bin/grub_x86_64.efi.

#. Preparing the boot device

   .. code-block:: none

      $ dd if=/dev/zero of=zephyr.img bs=1M count=1024
      $ mkfs.vfat -F 32 zephyr.img
      $ losetup -f -P --show zephyr.img
      $ mount /dev/loop0 /mnt

   Create the following directories.

   :file:`/mnt/efi`

   :file:`/mnt/efi/boot`

   :file:`/mnt/kernel/`

   Copy ``zephyr.elf`` to ``/mnt/kernel/`` and ``grub_x86_64.efi`` to ``/mnt/efi/boot/``.

   Create ``/mnt/efi/boot/grub.cfg`` containing the following:

   .. code-block:: console

     set default=0
     set timeout=10

     menuentry "Zephyr Kernel" {
         multiboot /kernel/zephyr.elf
     }

   .. note::
      ``zephyr.elf`` locates in the ``build/zephyr/``. For example, we can find ``zephyr.elf`` in
      ``samples/hello_world/build/zephyr/zephyr.elf`` if we build a ``Hello World App``.

#. Follow `Getting started guide for Intel NUC <https://projectacrn.github.io/latest/getting-started/apl-nuc.html>`_ to
   boot "The ACRN Service OS"

#. Boot Zephyr as User OS

   Make one new directory ``zephyr`` in SOS. And then copy zephyr.img, ``/usr/share/acrn/samples/nuc/launch_zephyr.sh``
   and ``/usr/share/acrn/samples/bios/`` to ``zephyr`` directory. We can find ``OVMF.fd`` and ``launch_zephyr.sh`` in
   `ACRN Repo <https://github.com/projectacrn/acrn-hypervisor>`_ also.

   Run the ``launch_zephyr.sh`` script to launch Zephyr as UOS.

   .. code-block:: none

      $ ./launch_zephyr.sh

   Load Zephyr using ``grub_x86_64.efi`` in UEFI shell:

   .. code-block:: console

      $ shell> FS0:
      $ FS0:\> efi\boot\grub_x86_64.efi

   Then zephyr will boot up automatically. You will see the prompt as following:

   .. code-block:: console

      uart:~$
