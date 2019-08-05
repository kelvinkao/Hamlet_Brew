/*===========================================================================

FILE: Hamlet.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "AEEFile.h"			// File interface definitions
#include "AEEMenu.h"		// Menu Services
#include "AEEText.h"

#include "Hamlet.bid"
#include "Hamlet.brh"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pHam->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _Hamlet {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  di; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // my own variables
	//for images
	IImage* pImageLogo;
	IImage*	pImageBack;
	IImage* pImageWall;
	IImage* pImageHamlet;
	IImage* pImageGertrude;
	IImage* pImageDead;
	IImage* pImageBastard;
	IImage* pImageSword;

	//scene control
	int nLevel;
	int nBranch;
	int nAnimTemp;

	//menu
	IMenuCtl	* pIMenu;
	IStatic		* pIStatic;
	
	AECHAR szTextBuf[256];
} Hamlet;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean Hamlet_HandleEvent(Hamlet* pHam, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean Hamlet_InitAppData(Hamlet* pHam);
void    Hamlet_FreeAppData(Hamlet* pHam);

void Hamlet_Timer(Hamlet* pHam);
void Hamlet_ShowLogo(Hamlet* pHam);
void Hamlet_ShowInstructions(Hamlet* pHam);
void Hamlet_BuildLevel3(Hamlet* pHam);
void Hamlet_BuildLevel4(Hamlet* pHam);
void Hamlet_BuildLevel5(Hamlet* pHam);
void Hamlet_BuildLevel5Frame2(Hamlet* pHam);
void Hamlet_BuildLevel5Frame3(Hamlet* pHam);
void Hamlet_BuildLevel6(Hamlet* pHam);
void Hamlet_BuildLevel6Frame2(Hamlet* pHam);
void Hamlet_BuildLevel6Frame3(Hamlet* pHam);
void Hamlet_BuildLevel7(Hamlet* pHam);

void Hamlet_BuildStatic(Hamlet* pHam);
void EndlinizeString(AECHAR*);	//replace ^ with \n
void Hamlet_BuildMenu(Hamlet* pHam);
void Hamlet_DrawScenery(Hamlet* pHam);	//draws the background and the wall
void Hamlet_DrawCharacters(Hamlet* pHam);	//draws Hamlet and Gertrude

enum 
{
	MENUID_POLONIUS,
	MENUID_KENNY,
	MENUID_SPLINTER,
};

#define LEVEL1_DELAY 2000
#define LEVEL2_DELAY 3000
#define LEVEL3_DELAY 10000
#define LEVEL5_DELAY 2000
#define LEVEL6_DELAY 4000
#define LEVEL7_DELAY 5000

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell object. 

	pIModule: pin]: Contains pointer to the IModule object to the current module to which
	this app belongs

	ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
	of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_HAMLET_BID )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(Hamlet),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)Hamlet_HandleEvent,
                          (PFNFREEAPPDATA)Hamlet_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(Hamlet_InitAppData((Hamlet*)*ppObj))
			{
				//Data initialized successfully
				return(AEE_SUCCESS);
			}
			else
			{
				//Release the applet. This will free the memory allocated for the applet when
				// AEEApplet_New was called.
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}

        } // end AEEApplet_New

    }

	return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function. All APPs must supply an Event Handler.

PROTOTYPE:
	boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
	pi: Pointer to the AEEApplet structure. This structure contains information specific
	to this applet. It was initialized during the AEEClsCreateInstance() function.

	ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean Hamlet_HandleEvent(Hamlet* pHam, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	AEEApplet * pMe = &pHam->a;
	//AECHAR szBuf[] = {'H','e','l','l','o',' ','W','o', 'r', 'l', 'd', '\0'}; //wide-character string

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:
		    Hamlet_Timer(pHam);

            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...
			ISHELL_CancelTimer(pHam->a.m_pIShell,NULL,pHam);
			if(pHam->nLevel>1)
			{	pHam->nLevel--;	}
      		return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
			Hamlet_Timer(pHam);
      		return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...

      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
			if(pHam->nLevel == 5 && pHam->pIMenu != NULL)
			{
				IMENUCTL_HandleEvent(pHam->pIMenu, EVT_KEY, wParam, 0);
			}	

			else if(wParam >= AVK_1 && wParam <= AVK_6)
			{
				switch(wParam)
				{
					case AVK_1:
						if(pHam->pImageBack)	{	IIMAGE_Release(pHam->pImageBack);		}
						pHam->pImageBack = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_BACK1);
						break;
					case AVK_2:
						if(pHam->pImageBack)	{	IIMAGE_Release(pHam->pImageBack);		}
						pHam->pImageBack = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_BACK2);
						break;
					case AVK_3:
						if(pHam->pImageBack)	{	IIMAGE_Release(pHam->pImageBack);		}
						pHam->pImageBack = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_BACK3);
						break;
					case AVK_4:
						if(pHam->pImageWall)	{	IIMAGE_Release(pHam->pImageWall);		}
						pHam->pImageWall = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_WALL1);
						break;
					case AVK_5:
						if(pHam->pImageWall)	{	IIMAGE_Release(pHam->pImageWall);		}
						pHam->pImageWall = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_WALL2);
						break;
					case AVK_6:
						if(pHam->pImageWall)	{	IIMAGE_Release(pHam->pImageWall);		}
						pHam->pImageWall = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_WALL3);
						break;

				}//end switch
				
				//update only at correct levels: note nLevel is set to the next level
				if(pHam->nLevel >= 4 && pHam->nLevel <= 7)
				{
					Hamlet_DrawScenery(pHam);
					Hamlet_DrawCharacters(pHam);
					IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings
				}
			}//end if
			//else if(pHam->nLevel == 5 && pHam->pIMenu != NULL)
			//{
			//	IMENUCTL_HandleEvent(pHam->pIMenu, EVT_KEY, wParam, 0);
			//}
			return (TRUE);
			
		//something from the menu is selected
		case EVT_COMMAND:
			pHam->nBranch = wParam;
			Hamlet_Timer(pHam);
			break;

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean Hamlet_InitAppData(Hamlet* pHam)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pHam->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pHam->di.wStructSize = sizeof(pHam->di);
    ISHELL_GetDeviceInfo(pHam->a.m_pIShell,&pHam->di);

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pHam->" without the "a."
    pHam->pIDisplay = pHam->a.m_pIDisplay;
    pHam->pIShell   = pHam->a.m_pIShell;

    // Insert your code here for initializing or allocating resources...
	pHam -> nLevel = 1;
	pHam -> nBranch = 0;
	pHam->pImageBack = ISHELL_LoadResImage(pHam->a.m_pIShell, HAMLET_RES_FILE, IMG_BACK0);
	pHam->pImageWall = ISHELL_LoadResImage(pHam->a.m_pIShell, HAMLET_RES_FILE, IMG_WALL0);

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void Hamlet_FreeAppData(Hamlet* pHam)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pHam->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pHam->pIMenuCtl)   // release the interface
    //    pHam->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

	//free the pictures
	if(pHam->pImageLogo)
	{	IIMAGE_Release(pHam->pImageLogo);		}
	if(pHam->pImageBack)
	{	IIMAGE_Release(pHam->pImageBack);		}
	if(pHam->pImageWall)
	{	IIMAGE_Release(pHam->pImageWall);		}
	if(pHam->pImageHamlet)
	{	IIMAGE_Release(pHam->pImageHamlet);		}
	if(pHam->pImageGertrude)
	{	IIMAGE_Release(pHam->pImageGertrude);	}
	if(pHam->pImageDead)
	{	IIMAGE_Release(pHam->pImageDead);		}
	if(pHam->pImageBastard)
	{	IIMAGE_Release(pHam->pImageBastard);	}
	if(pHam->pImageSword)
	{	IIMAGE_Release(pHam->pImageSword);		}

	//free GUI stuff
	if(pHam->pIMenu)
	{	IMENUCTL_Release(pHam->pIMenu);		}
	if(pHam->pIStatic)
	{	ISTATIC_Release(pHam->pIStatic);	}


}

//calls whatever appropriate when it's that level
void Hamlet_Timer(Hamlet* pHam)
{
	switch(pHam->nLevel)
	{
		case 1:
			Hamlet_ShowLogo(pHam);
			break;
		case 2:
			Hamlet_ShowInstructions(pHam);
			break;
		case 3:
			Hamlet_BuildLevel3(pHam);
			break;
		case 4:
			Hamlet_BuildLevel4(pHam);
			break;
		case 5:
			Hamlet_BuildLevel5(pHam);
			break;
		case 6:
			Hamlet_BuildLevel6(pHam);
			break;
		case 7:
			Hamlet_BuildLevel7(pHam);
			break;
	}
}

//display the "Hamlet" logo, level 1
void Hamlet_ShowLogo(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;
	pHam->nLevel = 1;

	IDISPLAY_ClearScreen(pMe->m_pIDisplay);
	pHam->pImageLogo = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_LOGO);
	IIMAGE_SetParm(pHam->pImageLogo, IPARM_ROP, AEE_RO_TRANSPARENT, 0 );
	IIMAGE_Draw(pHam->pImageLogo, 1, 50);
	IDISPLAY_Update(pMe->m_pIDisplay);
	IIMAGE_Release(pHam->pImageLogo);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL1_DELAY, (PFNNOTIFY)Hamlet_Timer, pHam);
	pHam->nLevel = 2;
}

//show instructions about what to press, level 2
void Hamlet_ShowInstructions(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;
	AECHAR strBuf[40];

	//clear screen (default color is white)
	IDISPLAY_ClearScreen(pMe->m_pIDisplay);

	//draw the text
	ISHELL_LoadResString(pMe->m_pIShell, HAMLET_RES_FILE, INSTRUCTION0, strBuf, 40);
    IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, strBuf, -1, 20, 30, 0, NULL);
	ISHELL_LoadResString(pMe->m_pIShell, HAMLET_RES_FILE, INSTRUCTION1, strBuf, 40);
    IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, strBuf, -1, 20, 50, 0, NULL);
	ISHELL_LoadResString(pMe->m_pIShell, HAMLET_RES_FILE, INSTRUCTION2, strBuf, 40);
    IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, strBuf, -1, 20, 65, 0, NULL);
	ISHELL_LoadResString(pMe->m_pIShell, HAMLET_RES_FILE, INSTRUCTION3, strBuf, 40);
    IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, strBuf, -1, 20, 85, 0, NULL);
	ISHELL_LoadResString(pMe->m_pIShell, HAMLET_RES_FILE, INSTRUCTION4, strBuf, 40);
    IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, strBuf, -1, 20, 100, 0, NULL);
	
	//update screen
    IDISPLAY_Update(pMe->m_pIDisplay);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL2_DELAY, (PFNNOTIFY)Hamlet_Timer, pHam);
	pHam->nLevel = 3;
}

//the level where Hamlet tells Gertrude about the rat
void Hamlet_BuildLevel3(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	//set the background and wall to init value
	IDISPLAY_ClearScreen(pMe->m_pIDisplay);

	//set the characters
	pHam->pImageHamlet = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_HAMLET);
	IIMAGE_SetParm(pHam->pImageHamlet, IPARM_ROP, AEE_RO_TRANSPARENT, 0 );
	pHam->pImageGertrude = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_GERTRUDE);
	IIMAGE_SetParm(pHam->pImageGertrude, IPARM_ROP, AEE_RO_TRANSPARENT, 0);

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//static text box
	Hamlet_BuildStatic(pHam);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL3_DELAY, (PFNNOTIFY)Hamlet_Timer, pHam);
	pHam->nLevel = 4;
}

//the level where user gets to choose who Hamlet kills
void Hamlet_BuildLevel4(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	if (pHam->pIStatic)
	{
		ISTATIC_Release(pHam->pIStatic);
		pHam->pIStatic = NULL;
	}

	IDISPLAY_ClearScreen(pMe->m_pIDisplay);

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//go to next screen after recording selection
	//go to next screen
	pHam->nLevel = 5;

	//static text box
	Hamlet_BuildMenu(pHam);

}

//the level where Hamlet stabs 
void Hamlet_BuildLevel5(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IDISPLAY_ClearScreen(pMe->m_pIDisplay);

	pHam->pImageSword = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SWORD1);

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//static text box
	Hamlet_BuildStatic(pHam);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, 500, (PFNNOTIFY)Hamlet_BuildLevel5Frame2, pHam);
}

void Hamlet_BuildLevel5Frame2(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IIMAGE_Release(pHam->pImageSword);
	pHam->pImageSword = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SWORD2);

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, 500, (PFNNOTIFY)Hamlet_BuildLevel5Frame3, pHam);
}

void Hamlet_BuildLevel5Frame3(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IIMAGE_Release(pHam->pImageSword);
	pHam->pImageSword = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SWORD3);

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL5_DELAY, (PFNNOTIFY)Hamlet_BuildLevel6, pHam);
	pHam->nLevel = 6;
}

//the level where the rat is revealed
void Hamlet_BuildLevel6(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IDISPLAY_ClearScreen(pMe->m_pIDisplay);
	switch(pHam->nBranch)
	{
		case MENUID_POLONIUS:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_POLONIUS1);
			break;
		case MENUID_KENNY:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_KENNY1);
			break;
		case MENUID_SPLINTER:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SPLINTER1);
			break;
	}

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//static text box
	Hamlet_BuildStatic(pHam);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, 200, (PFNNOTIFY)Hamlet_BuildLevel6Frame2, pHam);
}

//the level where the rat is revealed
void Hamlet_BuildLevel6Frame2(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IIMAGE_Release(pHam->pImageDead);
	switch(pHam->nBranch)
	{
		case MENUID_POLONIUS:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_POLONIUS2);
			break;
		case MENUID_KENNY:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_KENNY2);
			break;
		case MENUID_SPLINTER:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SPLINTER2);
			break;
	}

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, 300, (PFNNOTIFY)Hamlet_BuildLevel6Frame3, pHam);
}

//the level where the rat is revealed
void Hamlet_BuildLevel6Frame3(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IIMAGE_Release(pHam->pImageDead);
	switch(pHam->nBranch)
	{
		case MENUID_POLONIUS:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_POLONIUS3);
			break;
		case MENUID_KENNY:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_KENNY3);
			break;
		case MENUID_SPLINTER:
			pHam->pImageDead = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_SPLINTER3);
			break;
	}

	Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
	Hamlet_DrawCharacters(pHam);	//draw the characters
	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL6_DELAY, (PFNNOTIFY)Hamlet_Timer, pHam);
	pHam->nLevel = 7;
}

//conclusion
void Hamlet_BuildLevel7(Hamlet* pHam)
{
	AEEApplet * pMe = &pHam->a;

	IDISPLAY_ClearScreen(pMe->m_pIDisplay);

	switch(pHam->nBranch)
	{
		case MENUID_POLONIUS:
			Hamlet_DrawScenery(pHam);		//draw the scenerary (background behind wall)
			Hamlet_DrawCharacters(pHam);	//draw the characters
			pHam->pImageBastard = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_TEARDROPS);
			IIMAGE_SetParm(pHam->pImageBastard, IPARM_ROP, AEE_RO_TRANSPARENT, 0 );
			IIMAGE_Draw(pHam->pImageBastard, 69, 38);
			break;
		case MENUID_KENNY:
			pHam->pImageBastard = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_STANKYLE);
			IIMAGE_Draw(pHam->pImageBastard, 0, 0);
			break;
		case MENUID_SPLINTER:
			pHam->pImageBastard = ISHELL_LoadResImage(pMe->m_pIShell, HAMLET_RES_FILE, IMG_TURTLES);
			IIMAGE_Draw(pHam->pImageBastard, 0, 0);
			break;
	}

	IDISPLAY_Update(pHam->a.m_pIDisplay);	//update all drawings

	//static text box
	Hamlet_BuildStatic(pHam);

	//go to next screen
	ISHELL_SetTimer(pMe->m_pIShell, LEVEL7_DELAY, (PFNNOTIFY)Hamlet_Timer, pHam);
	pHam->nLevel = 8;
}

//the static textbox to be used for levels 3, 5, 6, 7
void Hamlet_BuildStatic(Hamlet* pHam)
{
	AEERect qrc;
	AECHAR szTitle[32];
	int textSelect = 0;

	if(pHam->pIStatic == NULL)
	{
		ISHELL_CreateInstance(pHam->a.m_pIShell, AEECLSID_STATIC, (void **)&pHam->pIStatic);	

		//make dimensions of the control take up the lower half of screen
		qrc.x	= 3;
		qrc.y	= 85;
		qrc.dx	= pHam->di.cxScreen;
		qrc.dy	= pHam->di.cyScreen;

		ISTATIC_SetRect(pHam->pIStatic, &qrc);	//lower half of screen
	}

	//load up title text in our title character array
	ISHELL_LoadResString(pHam->a.m_pIShell, HAMLET_RES_FILE, STAT_TITLE, szTitle, sizeof(szTitle));		 

	//load up our text in a global buffer
	switch(pHam->nLevel)
	{
		case 3:
			textSelect = TEXT_LEVEL3;
			break;
		case 5:
			textSelect = TEXT_LEVEL5;
			break;
		case 6:
			switch(pHam->nBranch)
			{
				case MENUID_POLONIUS:
					textSelect = TEXT_LEVEL6_POLONIUS;
					break;
				case MENUID_KENNY:
					textSelect = TEXT_LEVEL6_KENNY;
					break;
				case MENUID_SPLINTER:
					textSelect = TEXT_LEVEL6_SPLINTER;
					break;
			}
			break;
		case 7:
			switch(pHam->nBranch)
			{
				case MENUID_POLONIUS:
					textSelect = TEXT_LEVEL7_POLONIUS;
					break;
				case MENUID_KENNY:
					textSelect = TEXT_LEVEL7_KENNY;
					break;
				case MENUID_SPLINTER:
					textSelect = TEXT_LEVEL7_SPLINTER;
					break;
			}
			break;
	}
	ISHELL_LoadResString(pHam->a.m_pIShell, HAMLET_RES_FILE, textSelect, pHam->szTextBuf, sizeof(pHam->szTextBuf));
	EndlinizeString(pHam->szTextBuf);
	
	ISTATIC_SetText(pHam->pIStatic, szTitle, pHam->szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
		
	//draw control
	ISTATIC_Redraw(pHam->pIStatic);
}

//turns '^' into '\n'
void EndlinizeString(AECHAR* str)
{
	int index = -1;
	while(str[++index]!='\0')
	{
		if(str[index] == '^')
		{
			str[index] = '\n';
		}
	}
}

void Hamlet_BuildMenu(Hamlet* pHam)
{
	AEERect qrc;
	//make dimensions of the control take up the entire screen
	qrc.x	= 0;
	qrc.y	= 85;
	qrc.dx	= pHam->di.cxScreen;
	qrc.dy	= pHam->di.cyScreen;
	
	//create the menu
	if(pHam->pIMenu == NULL)
	{
		ISHELL_CreateInstance(pHam->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pHam->pIMenu);
	}

	IMENUCTL_SetRect(pHam->pIMenu, &qrc);	//lower half of screen

	//set up the menu
	IMENUCTL_SetTitle(pHam->pIMenu, HAMLET_RES_FILE, STR_MENUTITLE, NULL);

	//Add in our menu items
	IMENUCTL_AddItem(pHam->pIMenu, HAMLET_RES_FILE, STR_POLONIUS, MENUID_POLONIUS, NULL, 0);
	IMENUCTL_AddItem(pHam->pIMenu, HAMLET_RES_FILE, STR_KENNY, MENUID_KENNY, NULL, 0);
	IMENUCTL_AddItem(pHam->pIMenu, HAMLET_RES_FILE, STR_SPLINTER, MENUID_SPLINTER, NULL, 0);

	IMENUCTL_SetActive(pHam->pIMenu,TRUE);
}

//draws the background and the wall
void Hamlet_DrawScenery(Hamlet* pHam)
{
	IIMAGE_Draw(pHam->pImageBack, 85, 25);
	IIMAGE_SetParm(pHam->pImageWall, IPARM_ROP, AEE_RO_TRANSPARENT, 0 );
	IIMAGE_Draw(pHam->pImageWall, 0, 0);
}

//draws hamlet and gertrude
void Hamlet_DrawCharacters(Hamlet* pHam)
{
	IIMAGE_Draw(pHam->pImageHamlet, 21, 42);
	IIMAGE_Draw(pHam->pImageGertrude, 53, 27);
	if(pHam->nLevel == 5 || pHam->nLevel == 6)
	{
		IIMAGE_SetParm(pHam->pImageSword, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
		IIMAGE_Draw(pHam->pImageSword, 5, 52);
	}
	
	if(pHam->nLevel == 6 || (pHam->nLevel > 6 && pHam->nBranch == MENUID_POLONIUS))
	{
		IIMAGE_SetParm(pHam->pImageDead, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
		IIMAGE_Draw(pHam->pImageDead, 0, 37);
	}
}