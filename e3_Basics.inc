Sub basics_Setup
   /declare clippedDown bool outer FALSE
   /declare expAmt int outer ${Me.Exp}
   /declare aaExpAmt int outer ${Me.AAExp}

   /declare roamTargetID int outer 0
   /declare roamMode bool outer FALSE
   /declare roamRefreshTimer timer outer 0

   /declare meditateOn bool outer FALSE
   /declare meditateBlockTimer timer outer

   /declare looter bool outer FALSE
   /declare lootEntries[128,2] string outer -

   /declare backgroundEventsCheckTimer timer outer 0
   /declare cancelLootTimer timer outer 0
   /declare checkLooterTimer timer outer 0

   /declare backgroundFPS int outer 0
   /declare foregroundFPS int outer 0
   /declare enageClipDown bool outer TRUE
   /declare renderFG int outer 0
   /declare renderBG int outer 0
   /declare actorClipPlane int outer 0

   | INI SETTINGS
   /declare expSpam bool outer

   /call getIniToVar "${iniFileCharacter},Misc,EXP Spam" expSpam TRUE
   /call getIniToVar "${iniFileCharacter},Misc,Looter" looter FALSE
   /call getIniToVar "${iniFileCharacter},System,BackgroundFPS" backgroundFPS 30
   /call getIniToVar "${iniFileCharacter},System,ForegroundFPS" foregroundFPS 60
   /call getIniToVar "${iniFileCharacter},System,EnageClipDown" enageClipDown FALSE
   /call getIniToVar "${iniFileCharacter},System,ActorClipPlane" actorClipPlane 50
   /call getIniToVar "${iniFileCharacter},System,RenderFG" renderFG 1
   /call getIniToVar "${iniFileCharacter},System,RenderBG" renderBG 2

   /if (${looter}==TRUE) {
      /hidec looted
      /call loadLootIni
   }

   | SET CURRENT XP VALUES
   /varcalc expAmt (${Me.Level}*100) + ${Me.PctExp}
   /varcalc aaExpAmt (${Me.AAPointsSpent} + ${Me.AAPoints} + ${Math.Calc[${Me.PctAAExp}/100]})
/return

SUB basics_CharacterSettings(bool backFillOnly)
   /call WriteToIni "${iniFileCharacter},Misc,EQBC Channels" "team" ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Misc,EXP Spam" YES ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Misc,Looter" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,BackgroundFPS" 30 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,ForegroundFPS" 60 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,EnageClipDown" FALSE ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,ActorClipPlane" 50 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,RenderFG" 1 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},System,RenderBG" 2 ${backFillOnly}
/RETURN

#EVENT EndMacro "It will take about 5 more seconds to prepare your camp."
SUB EVENT_EndMacro
   /endmacro
/RETURN


#EVENT skillup "#*#You have become better at #1#! (#2#)#*#"
SUB EVENT_skillup(str line, str skillName, str skillValue)
   /bc Skill Improved: ${skillName} -> ${skillValue}
/RETURN





|------------------------------------------------|
|- Engages MedBreak mode, on bots who can cast. -|
|------------------------------------------------|
#EVENT medOn "[#1#] MedBreak On#*#"
#EVENT medOn "<#1#> MedBreak On#*#"
#EVENT medOn "#1# tells you, 'MedBreak On#*#'"
#EVENT medOn "#1# tells the group, 'MedBreak On#*#'"
#EVENT medOn "#1# you tell your party, 'MedBreak On#*#'"
SUB EVENT_medOn(line, ChatSender)

   | Check that I'm a caster, and not a bard
   /if (${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[BRD]}) {
      /call activateMeditateMode
   }
/RETURN


SUB activateMeditateMode
   /varset meditateOn TRUE
/RETURN



|----------------------------------------------------|
|- disengages MedBreak mode, on bots who can cast. -|
|----------------------------------------------------|
#EVENT medOff "[#1#] MedBreak Off#*#"
#EVENT medOff "<#1#> MedBreak Off#*#"
#EVENT medOff "#1# tells you, 'MedBreak Off#*#'"
#EVENT medOff "#1# tells the group, 'MedBreak Off#*#'"
#EVENT medOff "#1# you tell your party, 'MedBreak Off#*#'"
SUB EVENT_medOff(line, ChatSender)
   /call deactivateMeditateMode
/RETURN


SUB deactivateMeditateMode
   /if (${meditateOn}==FALSE) {
      /if (${Me.Sitting}==TRUE) /stand
      /return
   }

   /bc Ending Medbreak.
   /varset meditateOn FALSE
   /if (${Me.Sitting}) /stand
/RETURN


#EVENT medOffFromHit "#*# YOU for#*#points of damage."
SUB EVENT_medOffFromHit
   /if (${meditateOn}==FALSE) /return

   /if (${meditateBlockTimer} < 50) {
      /call pauseMedBreak 15s
   }
/RETURN



SUB check_MedBreak
   /call checkOpenWindows
   /if (${Macro.Return}==TRUE) /return
   
   /if (${assistTarget}!=0 && (${meleeAssist}==TRUE || ${autoFireAssist}==TRUE)) /return

   /if (${Me.Moving}) /return

   | PREVENT MEDDING AND STAND, IF THERES A TIMER ACTIVE FROM BEING HIT
   /if (${meditateBlockTimer}>0) {
      /if (${Me.Sitting}==TRUE) /stand
      /return
   }

   | MOUNTED? BAIL..
   /if (${Me.Casting.ID}!=NULL) /return

   /if (${Me.State.Equal[MOUNT]}==TRUE) /return

   /if (${Me.PctMana} >= 99) {
      /echo I am at full mana, pausing medbreak for 10s.
      /if (${Me.Sitting}) /stand
      /call pauseMedBreak 10s
      /return
   }

   /if (${Me.State.Equal[STAND]}==TRUE) {
      /timed 20 /if (${meditateBlockTimer}==0 && ${Me.Sitting}==FALSE && ${Me.Moving}==FALSE) /sit
      |/delay 15
   }


/RETURN

SUB pauseMedBreak(string waitTime)
   /if (${Defined[waitTime]}==FALSE) {
      /declare waitTime string local 10s
   }

   /echo Pausing Medidate: ${waitTime}
   /varset meditateBlockTimer ${waitTime}
   /if (${Me.Sitting}==TRUE) /stand
/RETURN


Sub basics_Background_Events
   /if (${backgroundEventsCheckTimer}>0) /return
   /varset backgroundEventsCheckTimer 10

   /if (${looter}==TRUE) /call checkLoot
   /if (${meditateOn}==TRUE) /call check_MedBreak
   /if (${roamMode}==TRUE) /call check_Roam
   /if (${Stick.Active}==TRUE && ${enageClipDown}==TRUE) {
      /if (${clippedDown}==FALSE) {
         /notify OptionsWindow ODP_ClipPlaneSlider newvalue 1
         /varset clippedDown TRUE
      }
   } else {
      /if (${clippedDown}==TRUE && ${enageClipDown}==TRUE) {
         /notify OptionsWindow ODP_ClipPlaneSlider newvalue 6
         /varset clippedDown FALSE
      }
   }
/return

SUB loadLootIni
   /declare i int local 0
   /declare lootKeys string local ${Ini[loot,loot]}
   /declare entryName string local
   /declare entryValue string local
   /declare count int local 0


   | IMPORT LOOT ENTRIES INTO PERSISTENT ARRAY
   /for i 1 to 128
      /varset entryName ${lootKeys.Arg[${i},|]}
      /if (${entryName.Equal[NULL]}) {
         |/echo skip
         /goto :loopnext
      }
      /varset entryValue ${Ini[loot,loot,${entryName}]}
      /varset lootEntries[${i},1] ${entryName}
      /varset lootEntries[${i},2] ${entryValue}
      |/echo ${entryName} ${entryValue}
      /varset count ${Math.Calc[${count}+1]}
      :loopnext
   /next i

   /echo Loaded ${count} loot entries.


/RETURN

SUB checkLoot
   /if (${cancelLootTimer} > 0) /return
   /if (${combatTimer} > 0) /return
   /if (${checkLooterTimer} > 0) /return
   /if (${Stick.Active}==TRUE) /return

   /declare i int local 0
   /declare j int local 0
   /declare k int local 0
   /declare lootString string local 

   :BEGIN LOOT CYCLE
   /declare corpseIDs[128] int local
   /declare corpseCount int local ${SpawnCount[npccorpse LOS radius 100]}

   /if (${corpseCount}==0) /return

   /for k 1 to ${SpawnCount[npccorpse LOS radius 100]}
      /echo ${NearestSpawn[${k},npccorpse LOS radius 100].Name}
      /varset corpseIDs[${k}] ${NearestSpawn[${k},npccorpse LOS radius 100].ID}
   /next k

   /hidec looted

   /for k 1 to ${corpseCount}
      /call ensureNoLootWindow
      /call ensureNoItemDisplayWindows

      /stick 5 id ${corpseIDs[${k}]}
      /call waitForIntLTE "Spawn[id ${corpseIDs[${k}]}].Distance" 10
      /call waitForIntLTE "Me.Speed" 1
      /tar id ${corpseIDs[${k}]}

      /if (${Spawn[id ${corpseIDs[${k}]}].ID}==NULL) /goto :loopnextk
      /tar id ${corpseIDs[${k}]}
      /delay 2

      :tryLoot
         /doevents
         /if (${stopWait}) {
            /goto :loopnextk
            /varset stopWait FALSE
         }
         /loot
         /delay 3
         /echo .
         /if (${Spawn[id ${corpseIDs[${k}]}].ID}==NULL) /goto :loopnextk
         /if (${Target.ID}==NULL) /tar id ${corpseIDs[${k}]}
      /if (${Window[LootWnd]}==FALSE) /goto :tryLoot

      /delay 3
      /varset lootString Loot on ${corpseIDs[${k}]}:

      /for i 0 to 10
         /if (${Window[LootWnd].Child[LW_LootSlot${i}].Tooltip.Length} == 0) /next i

         /varset lootString ${lootString}, ${Window[LootWnd].Child[LW_LootSlot${i}].Tooltip}

         /for j 1 to 128
            /if (${lootEntries[${j},1].Equal[-]}) /goto :loopescj
            /if (${Window[LootWnd].Child[LW_LootSlot${i}].Tooltip.Find[${lootEntries[${j},1]}]}!=NULL) {
               /notify LootWnd LW_LootSlot${i} rightmouseup
               /bc Looting: ${Window[LootWnd].Child[LW_LootSlot${i}].Tooltip}
               /delay 3
            }
         /next j
         :loopescj

         /call ensureNoItemDisplayWindows
      /next i

      /bc e3-loot: ${lootString}
      /varset lootString
      /if (${cancelLootTimer}>0) /goto :loopesck

      :loopnextk
   /next k 
   :loopesck

   /call ensureNoLootWindow
   /call ensureNoItemDisplayWindows

   /varset cancelLootTimer 5s
   
/RETURN


Sub basics_Aliases

   /squelch /alias /medon /bc MedBreak On
   /squelch /alias /medoff /bc MedBreak Off
/return


sub check_Roam
   /if (${roamRefreshTimer}==0 && ${Spawn[id ${roamTargetID}].Distance3D}>30) {
      /if (${assistTarget}!=0) /call setAssistOff
      
      |/moveto loc ${Spawn[${roamTargetID}].Y} ${Spawn[${roamTargetID}].X} ${Spawn[${roamTargetID}].Z} 
      /if (${Navigation.Active}==FALSE) /nav id ${roamTargetID}
      /varset roamRefreshTimer 10
   }
/return

#EVENT roamWith "[#1#(msg)] e3 roam with #1#"
#EVENT roamWith "<#*#> e3 roam with #1#"
   SUB EVENT_roamWith(string line, string spawnSearch)
      /varset roamMode TRUE
      /varset roamTargetID ${Spawn[${spawnSearch}].ID}
      /bc I'll roam with ${roamTargetID}
   /RETURN

#EVENT stopRoam "[#1#(msg)] e3 roam off"
#EVENT stopRoam "<#*#> e3 roam off"
   SUB EVENT_stopRoam(string line, string spawnSearch)
      /varset roamMode FALSE
      /varset roamTargetID 0
      /bc Roam off.
   /RETURN

#EVENT cantLoot "#*#may not loot#*#"
SUB Event_cantLoot
   /bc test
   /varset stopWait TRUE
/RETURN

