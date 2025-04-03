
_Cla1Prog_Start = _Cla1funcsRunStart;

MEMORY
{
PAGE 0:    /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */

   RAML0_PRG   : origin = 0x008000, length = 0x000800		/* on-chip RAM block */
/*   RAML0BOOT_PRG   : origin = 0x008740, length = 0x000020	/* on-chip RAM block */
   RAMCLA_PRG  : origin = 0x009000, length = 0x001000      // for cla

   OTP         : origin = 0x3D7800, length = 0x000400     /* on-chip OTP */
   FLASH	   : origin = 0x3E8000, length = 0x00BFF0
   FLASHZ	   : origin = 0x3F3FF0, length = 0x000010	  /* codeEnd Data */
   FLASHA      : origin = 0x3F6000, length = 0x001F80
   CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */
   BEGIN       : origin = 0x3F7FF6, length = 0x000002     /* Part of FLASHA.  Used for "boot to Flash" bootloader mode. */
   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */

   IQTABLES    : origin = 0x3FE000, length = 0x000B50     /* IQ Math Tables in Boot ROM */
   IQTABLES2   : origin = 0x3FEB50, length = 0x00008C     /* IQ Math Tables in Boot ROM */
   IQTABLES3   : origin = 0x3FEBDC, length = 0x0000AA	  /* IQ Math Tables in Boot ROM */

   ROM         : origin = 0x3FF27C, length = 0x000D44     /* Boot ROM */
   RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM  */
   VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM  */

PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

//   RAMM0       : origin = 0x000000, length = 0x000400     /* on-chip RAM block M0 */
   RAMM1       : origin = 0x000400, length = 0x000200     /* on-chip RAM block M1 */
   /*RAML:origin = 0x000600, length = 0x000100      for BLACKBOXVariable */
   /*RAML:origin = 0x000700, length = 0x0000F0      for IsrVars */

   RAML0_DATA  : origin = 0x008800, length = 0x0007A0 // for cla change
//   RAML0_DATA_BOOT : origin = 0x008F00, length = 0x00a0
}

/* Allocate sections to memory blocks.
   Note:
         codestart user defined section in DSP28_CodeStartBranch.asm used to redirect code
                   execution when booting to flash
         ramfuncs  user defined section to store functions that will be copied from Flash into RAM
*/

SECTIONS
{

   bl_table            : > 0x3F7000,   PAGE = 0, type = DSECT  //FLASHA
   app_table           : > 0x3E8000,   PAGE = 0


   /* Allocate program areas: */
   .cinit              : > FLASH      PAGE = 0
   .pinit              : > FLASH      PAGE = 0
   .text               : > FLASH      PAGE = 0



   codestart           : > FLASH       PAGE = 0
   codeEnd			   : > FLASHZ	   PAGE = 0

   ramfuncs             : LOAD = FLASH,
                         RUN = RAML0_PRG,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
						 PAGE = 0

	 //for cla
    Cla1Prog           : LOAD = FLASH,
                         RUN = RAMCLA_PRG,
                         LOAD_START(_Cla1funcsLoadStart),
                         LOAD_END(_Cla1funcsLoadEnd),
                         RUN_START(_Cla1funcsRunStart),
						 PAGE = 0


   csmpasswds          : > CSM_PWL     PAGE = 0

   csm_rsvd            : > CSM_RSVD    PAGE = 0

   /* Allocate uninitalized data sections: */
   .stack              : > RAMM1       PAGE = 1
   .ebss               : > RAML0_DATA       PAGE = 1


/*   RamData             : > RAMM0       PAGE = 1 */
   /*.esysmem            : > RAMM0       PAGE = 1 */
   /* Initalized sections go in Flash */
   /* For SDFlash to program these, they must be allocated to page 0 */
   .econst             : > FLASH      PAGE = 0
   .switch             : > FLASH      PAGE = 0
   .const              : > FLASH      PAGE = 0
   
   /* Allocate IQ math areas: */
   IQmath              : > FLASH       PAGE = 0                  /* Math Code */
   IQmathTables        : > IQTABLES,   PAGE = 0, TYPE = NOLOAD

   /* .reset is a standard section used by the compiler.  It contains the */
   /* the address of the start of _c_int00 for C Code.   /*
   /* When using the boot ROM this section and the CPU vector */
   /* table is not needed.  Thus the default type is set here to  */
   /* DSECT  */
   .reset              : > RESET,      PAGE = 0, TYPE = DSECT
   vectors             : > VECTORS     PAGE = 0, TYPE = DSECT

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/

