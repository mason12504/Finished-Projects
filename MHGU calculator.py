
# Mason Andersen
# 10-30-2021
# This program is a GUI to calculate weapon EFR for monster hunter
#

##CREATE CLASS FOR WEAPONS FOR PRODUCING MORE PROFILES#############

# import tkinter library for GUI, PIL for image handling
from tkinter import *
from PIL import ImageTk, Image

#define filepath for images
IMAGES_FOLDER_PATH = r"D:\Desktop\Programs\Python Programs\Random Projects\Monster Hunter Damage Calculator\Images"
######### GLOBAL VARIABLES ############

# universal X offset, keeps objects in the same line vertically
global xOff
global xOff2
xOff = 10
xOff2 = 350

#universal Y offset, space between objects
global yOff
yOff = 25

#global variables

global nameEntry
global rawEntry
global affEntry
global sharpVar
global bonusAtkVar
global bonusAtkPercent
global bonusCritVar
global critUpVar

global dmgCalcLabel

global generalFont
global generalFontSize

global widgetCount

# the number of widgets used to alter spacing with place()
widgetCount = 0

# set general font and size
generalFont = "Bahnschrift"
generalFontSize = 11

# set global bonuses and crit multiplier to default, set sharpResult to default
globalBonusAtk = 0
bonusAtkPercent = 0
globalBonusAff = 0
globalCritMult = 1.25
sharpResult = 0.0

########################################


## allows the user to select which game to use the calculator with
#  first function to run
def gameSelect():

    gameSelectWindow = Tk()
    xGeo = "450"
    gameSelectWindow.geometry(xGeo+"x570")
    gameSelectWindow.title("Monster Hunter Damage Calculator")
    
    # these need to be global or else the image does not display,
    # weird quirk with tkinter I believe
    global MHGUImage
    global MHRImage
    MHGUImage = ImageTk.PhotoImage(Image.open(IMAGES_FOLDER_PATH+ "\MHGU logo.png"))
    MHRImage = ImageTk.PhotoImage(Image.open(IMAGES_FOLDER_PATH + "\Monster Hunter Rise Logo.png"))
    
    gameSelectLabel = Label(gameSelectWindow, text = "Select Your Game", font = (generalFont, 18))
    gameSelectLabel.place(x = int(xGeo)/2, y = 35, anchor = "center")
    
    mhguButton = Button(gameSelectWindow,
                        image = MHGUImage,
                        command = lambda:[gameSelectWindow.destroy(), main("MHGU")],
                        )
    
    mhguButton.place(x = int(xGeo)/2, y = 200, anchor = "center")

    mhRiseButton = Button(gameSelectWindow,
                          image = MHRImage,
                          command = lambda:[gameSelectWindow.destroy(), main("MHR")],
                          )
    mhRiseButton.place(x = int(xGeo)/2, y = 450, anchor = "center")

    gameSelectWindow.mainloop()

    
    
def main(game):

    global widgetCount
    
    # create main window
    global mainWindow
    mainWindow = Tk()
    mainWindow.geometry("500x550")
    mainWindow.title(game + " Effective Raw Calculator")
    

    ### Create Entry Labels and entry objects ###

    # name of weapon as string entry

    WepNameLabel = createWepNameLabel()
    
    # raw damage entry as integer
    rawLabel = createRawLabel()  

    # sharpness entry as dropdown
    sharpLabel = createSharpLabel()

    # affinity entry as integer
    affLabel = createAffLabel()

    # bonus attack entry as dropdown
    bonusAtkLabel = createBonusAtkLabel(game)

    # bonus crit entry as dropdown
    bonusCritLabel = createBonusCritLabel(game)

    # crit up entry as dropdown, y/n
    critUpLabel = createCritUpLabel(game)

    ### done ###

    # damage calculation final output label

    # needs to be global or else the function thinks its local when you assign it
    global dmgCalcLabel
    dmgCalcLabel = Label(mainWindow, text = "0.0", font =(generalFont, 25))
    
    dmgCalcLabel.place(x = 350, y = 380, anchor = "center")
    
    dmgCalcLabel2 = Label(mainWindow, text = "Effective Damage:", font = (generalFont, 14))
    dmgCalcLabel2.place(x = 350, y = 345, anchor = "center")

    # calculate button

    calcButton = Button(mainWindow, text = "Calculate", command = lambda: dmgCalc("MHGU"),
                        font = (generalFont, 14), bg = "#DDFFDD")
    calcButton.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1

    # return to game select button

    gameSelectButton = Button(mainWindow, text = "Return to Game Select",
                              command = lambda:[resetWidgetCount(), mainWindow.destroy(), gameSelect()],
                              font = (generalFont, generalFontSize))
    
    gameSelectButton.place(x = 350, y = 480, anchor = "center")
    
    # create display value labels
    createDisplayLabels()

    # run the GUI                            
    mainWindow.mainloop()

############### LABEL CREATION FUNCTIONS ##############

#currently not doing anything with this, plan is to make profiles and save values with it
def createWepNameLabel():
    
    global widgetCount
    global nameEntry
    
    label = Label(mainWindow, text = "Weapon Name", font = (generalFont, generalFontSize))
    nameEntry = Entry(mainWindow)
    nameEntry.config(font = (generalFont, 9))
    label.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    nameEntry.place(x = xOff, y = yOff*widgetCount + 5)
    nameEntry.insert(0, "")
    widgetCount = widgetCount + 1

def createRawLabel():

    global widgetCount
    global rawEntry
    rawLabel = Label(mainWindow, text = "Raw Attack", font = (generalFont, generalFontSize))
    rawEntry = Entry(mainWindow)
    rawEntry.config(font = (generalFont, 9))
    rawLabel.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    rawEntry.place(x = xOff, y = yOff*widgetCount + 5)
    rawEntry.insert(0, "0")
    widgetCount = widgetCount + 1
    

### Sharpness Label is much more complex ###
# creates the sharpness entry dropdown menu, changes the background color according to entry
def createSharpLabel():

    global widgetCount
    global sharpVar
    global sharpEntry
    sharpLabel = Label(mainWindow, text = "Sharpness", font = (generalFont, generalFontSize))
    
    sharpVar = StringVar(mainWindow)
    sharpChoices = ["Red", "Orange", "Yellow", "Green", "Blue", "White", "Purple"]
    sharpVar.set("Yellow")
    fgColor = "#000000"
    sharpEntry = OptionMenu(mainWindow, sharpVar, *sharpChoices, command = sharpColorString)
    bgColor = sharpColor(sharpEntry)
    sharpEntry.config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                         activebackground = bgColor, activeforeground = fgColor)
    sharpEntry["menu"].config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                              postcommand = lambda: sharpColor(sharpEntry),
                                 activebackground = bgColor, activeforeground = "#FFFFFF")
    
    sharpLabel.place(x = xOff, y = widgetCount*yOff + 5)
    widgetCount = widgetCount + 1
    sharpEntry.place(x = xOff, y = widgetCount*yOff + 5)
    # after each dropdown menu the widget count should
    # increment twice
    widgetCount = widgetCount + 2

def sharpColorString(stringIn):

    global sharpEntry

    returnVal = ""
    
    if stringIn == "Red":
        returnVal = "#FF4444"
    
    elif stringIn == "Orange":
        returnVal = "#FF8800"
    
    elif stringIn == "Yellow":
        returnVal = "#FFFF00"

    elif stringIn == "Green":
        returnVal = "#45f248"

    elif stringIn == "Blue":
        returnVal = "#467ddb"

    elif stringIn == "White":
        returnVal = "#FFFFFF"

    elif stringIn == "Purple":
        returnVal = "#A386D9"
    
    sharpColor(sharpEntry)
    
# from text, get the hex value for a color
def sharpColor(entry):
    
    if sharpVar.get() == "Red":
        returnVal = "#FF4444"
    
    elif sharpVar.get() == "Orange":
        returnVal = "#FF8800"
    
    elif sharpVar.get() == "Yellow":
        returnVal = "#FFFF00"

    elif sharpVar.get() == "Green":
        returnVal = "#45f248"

    elif sharpVar.get() == "Blue":
        returnVal = "#467ddb"

    elif sharpVar.get() == "White":
        returnVal = "#FFFFFF"

    elif sharpVar.get() == "Purple":
        returnVal = "#A386D9"
    
    entry.config(bg = returnVal, activebackground = returnVal)
    entry["menu"].config(bg = returnVal, activebackground = returnVal)
    

    return returnVal

### End sharpness label code ###

# creates the affinity label and entry, places
def createAffLabel():

    global widgetCount
    global affEntry
    
    affLabel = Label(mainWindow, text = "Base Affinity (%)", font = (generalFont, generalFontSize))
    affEntry = Entry(mainWindow)
    affEntry.config(font = (generalFont, generalFontSize))
    affLabel.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    affEntry.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    affEntry.insert(0, "0")
    
# creates the bonus attack label and entry, etc. 
def createBonusAtkLabel(game):

    global widgetCount

    bgColor = "#FF9999"
    fgColor = "#000000"
    
    global bonusAtkVar
    bonusAtkLabel = Label(mainWindow, text = "Attack Up", font = (generalFont, generalFontSize))
                      
    bonusAtkVar = StringVar(mainWindow)
    
    if game == "MHGU" :
        bonusAtkChoices = ['None', 'S (+10)', 'M (+15)', 'L (+20)']
        bonusAtkVar.set('None')
        
    # choices change for monster hunter rise
    if game == "MHR" :
        bonusAtkChoices = ['None', '1 (+3)', '2 (+6)', '3 (+9)', '4 (+5% +7)',
                           '5 (+6% +8)', '6 (+8% +9)', '7 (+10% +10)']
        bonusAtkVar.set('None')
    
    bonusAtkEntry = OptionMenu(mainWindow, bonusAtkVar, *bonusAtkChoices)
    bonusAtkEntry.config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                         activebackground = bgColor, activeforeground = fgColor)
    bonusAtkEntry["menu"].config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                                 activebackground = bgColor, activeforeground = "#FFFFFF")
    bonusAtkLabel.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    bonusAtkEntry.place(x = xOff, y = yOff*widgetCount + 5)
    #dropdown
    widgetCount = widgetCount + 2

# creates the bonus crit label and entry etc.
def createBonusCritLabel(game):

    global widgetCount

    bgColor = "#81d4fa"
    fgColor = "#000000"

    global bonusCritVar
    bonusCritLabel = Label(mainWindow, text = "Critical Eye", font = (generalFont, generalFontSize))\
                      
    bonusCritVar = StringVar(mainWindow)
    
    if game == "MHGU":
        bonusCritChoices = ['None' , 'S (+10%)', 'M (+20%)', 'L (+30%)']
        bonusCritVar.set('None')
    
    if game == "MHR":
        bonusCritChoices = ['None' , '1 (+5%)', '2 (+10%)', '3 (+15%)',
                            '4 (+20%)', '5 (+25%)', '6 (+30%)', '7 (+40%)']
        bonusCritVar.set('None')
        
    bonusCritEntry = OptionMenu(mainWindow, bonusCritVar, *bonusCritChoices)
    bonusCritEntry.config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                         activebackground = bgColor, activeforeground = fgColor)
    bonusCritEntry["menu"].config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                                 activebackground = bgColor, activeforeground = "#FFFFFF")
    bonusCritLabel.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    bonusCritEntry.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 2

def createCritUpLabel(game):

    global widgetCount 

    bgColor = "#dcd0ff"
    fgColor = "#000000"

    LText = "Critical Up"

    if game == "MHR":
        LText = "Critical Boost"

    global critUpVar
    critUpLabel = Label(mainWindow, text = LText, font = (generalFont, generalFontSize))\
                      
    critUpVar = StringVar(mainWindow)
    if game == "MHGU":
        critUpChoices = ['No' , 'Yes']
        critUpVar.set('No')
    if game == "MHR":
        critUpChoices = ['None','1 (+5%)' , '2 (+10%)', '3 (+15%)']
        critUpVar.set('None')
        
    critUpEntry = OptionMenu(mainWindow, critUpVar, *critUpChoices)
    critUpEntry.config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                      activebackground = bgColor, activeforeground = fgColor)
    critUpEntry["menu"].config(font = (generalFont, generalFontSize), bg = bgColor, fg = fgColor,
                                 activebackground = bgColor, activeforeground = "#FFFFFF")
    critUpLabel.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 1
    critUpEntry.place(x = xOff, y = yOff*widgetCount + 5)
    widgetCount = widgetCount + 2

def createDisplayLabels():
    
    global displayTotalDamageLabel
    displayTotalDamageLabel = Label(mainWindow, text = "Total Attack: 0.0", font = (generalFont, generalFontSize))
    displayTotalDamageLabel.place(x = xOff2, y = (20 + yOff), anchor = "center")

    global displaySharpnessMultLabel
    displaySharpnessMultLabel = Label(mainWindow, text = "Sharpness Multiplier: 1.0x", font = (generalFont, generalFontSize))
    displaySharpnessMultLabel.place(x = xOff2, y = (20 + 2*yOff), anchor = "center")
    
    global displayTotalAffinityLabel
    displayTotalAffinityLabel = Label(mainWindow, text = "Total Affinity: 0.0%", font = (generalFont, generalFontSize))
    displayTotalAffinityLabel.place(x = xOff2, y = (20 + 3*yOff), anchor = "center")

    global displayCritMultLabel
    displayCritMultLabel = Label(mainWindow, text = "Critical Hit Multiplier: 1.25x", font = (generalFont, generalFontSize))
    displayCritMultLabel.place(x = xOff2, y = (20 + 4*yOff), anchor = "center")

    global displayEffectiveDmgLabel
    displayEffectiveDmgLabel = Label(mainWindow, text = "Effective Damage: 0.0", font = (generalFont, generalFontSize))
    displayEffectiveDmgLabel.place(x = xOff2, y = (20 + 5*yOff), anchor = "center")
    
########### GLOBAL VARIABLE GET FUNCTIONS #################################

# get the crit up value (crit multiplier)

def getCritBoost(game):


    ### I could change this to have a variable which is critUpVar.get()
    #   and just use that instead of running the function but for now im gonna leave it
    #   for consistency's sake.

    # Also, globalCritMult isnt global so its name should be changed.
    # problem for later though 
    if critUpVar.get() == 'Yes' or critUpVar.get() == '3 (+15%)' :
            globalCritMult = 0.4

    elif critUpVar.get() == 'No' or critUpVar.get() == 'None' :
            globalCritMult = 0.25

    elif critUpVar.get() == '1 (+5%)':
        globalCritMult = 0.3

    elif critUpVar.get() == '2 (+10%)':
        globalCritMult = 0.35
    

    return globalCritMult

# get sharpness multiplier
def sharpGet():
    
    if sharpVar.get() == "Red":
        sharpResult = 0.5
    
    elif sharpVar.get() == "Orange":
        sharpResult = 0.75
    
    elif sharpVar.get() == "Yellow":
        sharpResult = 1.0

    elif sharpVar.get() == "Green":
        sharpResult = 1.05

    elif sharpVar.get() == "Blue":
        sharpResult = 1.20

    elif sharpVar.get() == "White":
        sharpResult = 1.32

    elif sharpVar.get() == "Purple":
        sharpResult = 1.39

    return sharpResult

# get attack bonus
def globalAttackGet(atkVar):
    
    globalBonusAtk = 0
    global bonusAtkPercent
    

    ####### MHGU #######
    
    if atkVar.get() == 'S (+10)':
        globalBonusAtk = 10
    
    elif atkVar.get() == 'M (+15)':
        globalBonusAtk = 15
    
    elif atkVar.get() == 'L (+20)':
        globalBonusAtk = 20

    ###### MH RISE ######
        
    elif atkVar.get() == '1 (+3)':
        globalBonusAtk = 3

    elif atkVar.get() == '2 (+6)':
        globalBonusAtk = 6

    elif atkVar.get() == '3 (+9)':
        globalBonusAtk = 9

    elif atkVar.get() == '4 (+5% +7)':
        globalBonusAtk = 7
        bonusAtkPercent = 5

    elif atkVar.get() == '5 (+6% +8)':
        globalBonusAtk = 8
        bonusAtkPercent =  6

    elif atkVar.get() == '6 (+8% +9)':
        globalBonusAtk = 9
        bonusAtkPercent =  8
        
    elif atkVar.get() == '7 (+10% +10)':
        globalBonusAtk = 10
        bonusAtkPercent =  10

    elif atkVar.get() == 'None':
        globalBonusAtk =  0
        bonusAtkPercent = 0

    return globalBonusAtk
    

# get affinity bonus
def globalAffGet(critVar):

    globalBonusAff = 0

    ###### MHGU ########
    
    if critVar.get() == 'S (+10%)':
        globalBonusAff = 10
    
    elif critVar.get() == 'M (+20%)':
        globalBonusAff = 20

    elif critVar.get() == 'L (+30%)':
        globalBonusAff = 30

    ###### MH RISE ######

    elif critVar.get() == '1 (+5%)':
        globalBonusAff = 5
    
    elif critVar.get() == '2 (+10%)':
        globalBonusAff = 10

    elif critVar.get() == '3 (+15%)':
        globalBonusAff = 15

    elif critVar.get() == '4 (+20%)':
        globalBonusAff = 20

    elif critVar.get() == '5 (+25%)':
        globalBonusAff = 25

    elif critVar.get() == '6 (+30%)':
        globalBonusAff = 30

    elif critVar.get() == '7 (+40%)':
        globalBonusAff = 40

    # return final value

    return globalBonusAff

# general entry get function, returns the value in the given entry box.
def intEntryGet(entry):
    
    if entry.get() == "":
        entryText = 0
    else:
        entryText = entry.get()
        
    return int(entryText)



######################### MAIN DAMAGE CALCULATION ######################

# primary damage calculation function
def dmgCalc(game):
    global sharpEntry
    global outDmg
    # this needs checked
    if checkInput(rawEntry, affEntry) == False:
        outDmg = "Incorrect Entry:\n Raw Damage and Affinity must be Integer Values"
        dmgCalcLabel.config(text = str(outDmg), font = (generalFont, generalFontSize))
        return 0
    # this is all great
    totalRaw = totalRawCalc()
    totalAff = totalAffCalc()
    critUp = getCritBoost(game)
    
    outDmg = round(totalRaw + (totalAff*(totalRaw*critUp)), 7)

    dmgCalcLabel.config(text = str(outDmg), font = (generalFont, 25))
    displayValues(game)

    return outDmg


# calculates the total raw damage from bonus attack and base attack
def totalRawCalc():
    
    mainInput = intEntryGet(rawEntry) + globalAttackGet(bonusAtkVar)
    percentIncrease = (bonusAtkPercent*0.01) + 1
    totalRaw = (mainInput * percentIncrease) * sharpGet()

    return totalRaw

# same but affinity with base and bonus
def totalAffCalc():
    
    totalAff = round(((intEntryGet(affEntry)*0.01) + (globalAffGet(bonusCritVar))*0.01), 3)

    return totalAff

# checks if the input is valid from raw damage and affinity entries
def checkInput(entry, entry2):

    entryText = entry.get()
    entryText = entryText + entry2.get()

    if entryText.isnumeric() == False:
        return False

    else:
        return True

# changes the display values
def displayValues(game):
    
    text1 = "Total Attack: " + str(round(totalRawCalc()/sharpGet(),2)) 
    text2 = "Total Affinity: " + str(round((totalAffCalc()*100) , 1)) + "%"
    text3 = "Sharpness Multiplier: " + str(sharpGet()) + "x"
    text4 = "Critical Hit Multiplier: " + str(getCritBoost(game)+1) + "x"
    text5 = "Effective Damage: " + (str(outDmg))
    displayTotalDamageLabel.config(text = text1, bg = "#FF9999")
    displayTotalAffinityLabel.config(text = text2, bg = "#81d4fa")
    displaySharpnessMultLabel.config(text = text3, bg = sharpColor(sharpEntry))
    displayCritMultLabel.config(text = text4, bg = "#dcd0ff")
    displayEffectiveDmgLabel.config(text = text5)


#JUST FOR TESTING FUNCTIONS
def easyPrint():
    print("that was easy")

def resetWidgetCount():
    global widgetCount
    widgetCount = 0
    
# run the program
gameSelect()





    
     



       
        
        
