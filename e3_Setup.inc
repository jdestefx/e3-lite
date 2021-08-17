
#include e3_Assists.inc
#include e3_Background.inc
#include e3_Basics.inc
#include e3_BuffCheck.inc
#include e3_Casting.inc
#include e3_Heals.inc
#include e3_Utilities.inc

#include e3_Classes_Bard.inc
#include e3_Classes_Beastlord.inc
#include e3_Classes_Berserker.inc
#include e3_Classes_Cleric.inc
#include e3_Classes_Druid.inc
#include e3_Classes_Enchanter.inc
#include e3_Classes_Magician.inc
#include e3_Classes_Monk.inc
#include e3_Classes_Necromancer.inc
#include e3_Classes_Paladin.inc
#include e3_Classes_Ranger.inc
#include e3_Classes_Rogue.inc
#include e3_Classes_ShadowKnight.inc
#include e3_Classes_Shaman.inc
#include e3_Classes_Warrior.inc
#include e3_Classes_Wizard.inc


SUB e3_Setup(string iniSuffix)
   /declare macroVersion string local 1.57

   /echo e3-lite v${macroVersion} Starting.

   /declare iniFileCharacter string outer e3_bots\${MacroQuest.Server}-${Me.CleanName}${iniSuffix}.ini
   /declare manaClasses string outer CLR,DRU,ENC,PAL,BRD,WIZ,MAG,SHM,NEC,BST,SHD
   /declare manaRegenClasses string outer CLR,DRU,ENC,PAL,WIZ,MAG,SHM,BST,SHD
   /declare hasteClasses string outer MNK,BRD,ROG,WAR,RNG,PAL,BST,SHD,BER
   /declare meleeClasses string outer BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR
   /declare priestClasses string outer CLR,DRU,SHM
   /declare hybridClasses string outer PAL,BST,BRD,RNG
   /declare pureCasterClasses string outer ENC,MAG,NEC,WIZ
   /declare rangedClasses string outer CLR,DRU,SHM,ENC,MAG,NEC,WIZ
   /declare castWhileMovingClasses string outer ROG,MNK,BRD
   /declare tankClasses string outer WAR,PAL,SHD
   /declare knightClasses string outer PAL,SHD
   /declare classes_Nukes string outer CLR,DRU,NEC,WIZ,MAG,ENC,SHM,PAL,RNG,BST
   /declare classes_DoTs string outer CLR,BST,DRU,ENC,NEC,RNG,SHM,SHD
   /declare classes_Buffs string outer CLR,BRD,BER,BST,DRU,ENC,MAG,NEC,PAL,RNG,ROG,SHM,WAR,WIZ,SHD,MNK
   /declare classes_Heals string outer CLR,DRU,SHM,PAL,RNG,NEC
   /declare classes_MeleeAssist string outer BRD,PAL,SHD,MNK,BST,BER,ROG,WAR
   /declare classes_RangeAssist string outer WIZ,NEC,MAG,ENC,SHM,CLR,DRU
   /declare classes_PassiveAssist string CLR,DRU,SHM,ENC
   /declare canDoBuffs bool outer ${classes_Buffs.Find[${myClass}]}
   /declare canDoNukes bool outer ${classes_Nukes.Find[${myClass}]}
   /declare canDoDoTs bool outer ${classes_DoTs.Find[${myClass}]}
   /declare canCastWhileMoving bool outer ${castWhileMovingClasses.Find[${myClass}]}

   /declare flagNamesByIndex list outer
   /invoke ${flagNamesByIndex.Append[bNull,bName,bValue,bTargets,bExcludeTargets,bTargetOverLevel,bCastableDuringAssist,bSelfOverHP,bSelfUnderHP,bSelfOverMana,bSelfUnderMana,bCastableOnlyResting,bCastOnlyDuringAssist,bSelfUnderEnd,bSelfOverEnd,bNoCancelChecks,bRequireGroupInRange,bTargetUnderHP,bTargetOverHP,bNotIfShortBuff,bNotIfLongBuff,bRecastDelay,bXXXTargetOverLife,bXXXTargetUnderLife,bHealPct,bTargetRace,bCombatOnly,bTargetUnderLevel,bUseHealIndex,bRequireMobsInProximity,bForceSpellID,bRequireTargetClass,bRequireNoActiveDisc,bNoSitTimer,bRequireBodyType,bRequireDiseased,bRequireGroup,bRequireHurtPartyMembers,bRequireMaxMobsInProximity,bRequireAssistTarget,bRequirePoisoned,bDoCommand,bCheckLongBuffs,bCheckShortBuffs,bNotToT,bKeyword,bRebuffSeconds,bDoAbility,bActivateBandolier,bRevertToBandolier,bFreeze,bInstructRemove,bMaxResists,bExtendedStackCheck,bXTargetHeal,bNoCast,bIncludeGroup,bMemInGem,bXTargetOnlyHeal,bTargetType,bRequireCastIdle,bOnlyAfterTaunt,bTargetLifeManaRatio,bAmToT,bRequireCursed,bRequireAssistTag,bNotIfAssistTag,bRequireUW,bCancelAtHP,bAssumeSuccess,bBurnKey,bRequireHealIndexOver,bIgnoreTargetType,bNotIfMounted,bEnabled]}

   /declare i int local
   /for i 1 to ${Math.Calc[${flagNamesByIndex.Count}-1]}
      /declare ${flagNamesByIndex.Item[${i}]} string outer ${i}
   /next i

   |/declare bName string outer 1
   |/declare bValue string outer 2
   |/declare bTargets string outer 3
   |/declare bExcludeTargets string outer 4
   |/declare bTargetOverLevel string outer 6
   |/declare bCastableDuringAssist string outer 7
   |/declare bSelfOverHP string outer 8
   |/declare bSelfUnderHP string outer 9
   |/declare bSelfOverMana string outer 10
   |/declare bSelfUnderMana string outer 11
   |/declare bCastableOnlyResting string outer 12
   |/declare bCastOnlyDuringAssist string outer 13
   |/declare bSelfUnderEnd string outer 14
   |/declare bSelfOverEnd string outer 15
   |/declare bNoCancelChecks string outer 16
   |/declare bRequireGroupInRange string outer 17
   |/declare bTargetUnderHP string outer 18
   |/declare bTargetOverHP string outer 19
   |/declare bNotIfShortBuff string outer 20
   |/declare bNotIfLongBuff string outer 21
   |/declare bRecastDelay string outer 22
   |/declare bXXXTargetOverLife string outer 23
   |/declare bXXXTargetUnderLife string outer 24
   |/declare bHealPct string outer 25
   |/declare bTargetRace string outer 25
   |/declare bCombatOnly string outer 26
   |/declare bTargetUnderLevel string outer 27
   |/declare bUseHealIndex string outer 28
   |/declare bRequireMobsInProximity string outer 29
   |/declare bForceSpellID string outer 30
   |/declare bRequireTargetClass string outer 31
   |/declare bRequireNoActiveDisc string outer 32
   |/declare bNoSitTimer string outer 33
   |/declare bRequireBodyType string outer 34
   |/declare bRequireDiseased string outer 35
   |/declare bRequireGroup string outer 36
   |/declare bRequireHurtPartyMembers string outer 37
   |/declare bRequireMaxMobsInProximity string outer 38
   |/declare bRequireAssistTarget string outer 39
   |/declare bRequirePoisoned string outer 40
   |/declare bDoCommand string outer 41
   |/declare bCheckLongBuffs string outer 42
   |/declare bCheckShortBuffs string outer 43
   |/declare bNotToT string outer 44
   |/declare bKeyword string outer 45
   |/declare bRebuffSeconds string outer 46
   |/declare bDoAbility string outer 47
   |/declare bActivateBandolier string outer 48
   |/declare bRevertToBandolier string outer 49
   |/declare bFreeze string outer 50
   |/declare bInstructRemove string outer 51
   |/declare bMaxResists string outer 52
   |/declare bExtendedStackCheck string outer 53
   |/declare bXTargetHeal string outer 54
   |/declare bNoCast string outer 55
   |/declare bIncludeGroup string outer 56
   |/declare bMemInGem string outer 57
   |/declare bXTargetOnlyHeal string outer 58
   |/declare bTargetType string outer 59
   |/declare bRequireCastIdle string outer 60
   |/declare bOnlyAfterTaunt string outer 61
   |/declare bTargetLifeManaRatio string outer 62
   |/declare bAmToT string outer 63
   |/declare bRequireCursed string outer 64
   |/declare bRequireAssistTag string outer 65
   |/declare bNotIfAssistTag string outer 66
   |/declare bRequireUW string outer 67
   |/declare bCancelAtHP string outer 68
   |/declare bAssumeSuccess string outer 69
   |/declare bBurnKey string outer 70
   |/declare bRequireHealIndexOver string outer 71
   |/declare bIgnoreTargetType string outer 72
   |/declare bNotIfMounted string outer 73
   |/declare bEnabled string outer 74
   
   /declare autoKeywordIgnoreValues ON,OFF,YES,NO,NONE
   /declare iniSectionsAutoProcess string outer Buffs,Heals,AutoAbilities,Debuffs,AssistSpells,Burns,AggroAbilities,BuffRequests

   /declare allNetbotNames string outer

   /call loadIniSections
   /call gatherNetbotNames

   /if (${Bool[${Ini[${iniFileCharacter}]}]}==FALSE) /call makeCharSettingsIniFile

   /call check_Plugins
   /call ${Me.Class.ShortName}_setup
   /call basics_Setup
   /call background_Setup
   /call assist_Setup
   /call heals_Setup
   /call buffs_Setup
   /call casting_Setup
   
   /stick off
   /squelch /windowstate ShortDurationBuffWindow open
   /notify AdvancedDisplayOptionsWindow ADOW_MaxBGFPSSlider newvalue ${Math.Calc[${backgroundFPS}-9]}
   /notify AdvancedDisplayOptionsWindow ADOW_MaxFPSSlider newvalue ${Math.Calc[${foregroundFPS}-9]}
   /notify AdvancedDisplayOptionsWindow ADOW_ActorClipPlaneSlider newvalue ${actorClipPlane}

   /if (${Bool[${Plugin[mq2fps]}]}==TRUE) {
      /squelch /render bg ${renderBG}
      /squelch /render fg ${renderFG}
      /squelch /maxfps fg ${foregroundFPS}
      /squelch /maxfps bg ${backgroundFPS}
   }
   
   /squelch /stick set autopause off
   /squelch /moveto dist 3

   /squelch /netbots on
   /squelch /netbots send=on
   /squelch /netbots grab=on
      
   /if (${importPastTags.NotEqual[-]}) {
      /bc Startup INI added PAST tags: ${importPastTags.Lower}
      /invoke ${persistentAssistTags.Append[${importPastTags.Lower}]}
   }

   /echo e3-lite startup complete.

/RETURN

SUB loadIniSections
   /declare sections string local ${Ini[${iniFileCharacter}]}
   /declare a string local
   /declare i int local 1
   /declare sectionName string local
   /declare shortSectionName string local
   /declare newArrayName string local

   /for i 1 to ${sections.Count[|]}
      /varset sectionName ${sections.Token[${i},|]}

      /if (${sectionName.Length}==0 || ${Bool[${sectionName}]}==NULL) /goto :loopnexti
      /if (${Select[${sectionName},${iniSectionsAutoProcess}]}==0) /goto :loopnexti

      /varset shortSectionName ${sectionName.Replace[ ,]}
      /varset newArrayName ini${shortSectionName} 

      /declare ${newArrayName}[96,96] string outer -
      /declare tmpArray[96] string outer
      /call getIniSectionToArray "${iniFileCharacter},${shortSectionName}" tmpArray
      /call processArray tmpArray ${newArrayName}
      /deletevar tmpArray

      :loopnexti
   /next i

/RETURN


SUB gatherNetbotNames
   /declare i int local 
   /declare netbotCount int local ${Math.Calc[${NetBots.Counts}]}
   /declare netbotName string local
   /declare ret string local

   /for i 1 to ${netbotCount}
      /if (${ret.Length}==0) {
         /varset ret ${NetBots.Client[${i}]}
      } else {
         /varset ret ${ret}|${NetBots.Client[${i}]}
      }
   /next i

   /varset allNetbotNames ${ret}
/RETURN


SUB makeCharSettingsIniFile(bool backFillOnly)
   /declare autoAssistMode string local Off
   /if (${classes_MeleeAssist.Find[${myClass}]}!=NULL) /varset autoAssistMode On

   /echo Creating intial INI file for ${Me}...

   /call basics_CharacterSettings

   | Channels pre-config
   /if (${classes_MeleeAssist.Find[${myClass}]}!=NULL) /call WriteToIniAppend "${iniFileCharacter},Misc,EQBC Channels" "melee" " "
   /if (${classes_Heals.Find[${myClass}]}!=NULL) /call WriteToIniAppend "${iniFileCharacter},Misc,EQBC Channels" "healers" " "
   /if (${classes_RangeAssist.Find[${myClass}]}!=NULL || ${classes_PassiveAssist.Find[${myClass}]}!=NULL) /call WriteToIniAppend "${iniFileCharacter},Misc,EQBC Channels" "ranged"

   /call assist_CharacterSettings
   /call buffCheck_CharacterSettings
   /call heals_CharacterSettings
   /call ${Me.Class.ShortName}_CharacterSettings

/RETURN


|----------------------------------------------------|
|Checks for and enables required macro plugins.     -|
|----------------------------------------------------|
SUB check_Plugins
   /setwintitle ${Me.CleanName}
   
   /if (${Plugin[MQ2EQBC].Name.Length}==NULL) {
      /echo MQ2EQBC not loaded. Cannot continue. Ending.
      /end
   }

   /if (${Plugin[MQ2Collections].Name.Length}==NULL) {
      /echo MQ2Collections not loaded. Cannot continue. Ending.
      /end
   }

   /if (${Bool[${EQBC}]}) {
      /if (!${EQBC.Connected}) {
         /bccmd connect
         /delay 100 ${EQBC.Connected}
         /delay 5
         /if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS! Please open EQBCS and try again.  Macro functionality may be limited...
      }
   }
   
   /if (${Plugin[MQ2NetBots].Name.Length}==NULL) {
      /echo MQ2NetBots not loaded. Cannot continue. Ending.
      /end
   } else {
      /if (${NetBots}) {
         /squelch /netbots on grab=on send=on
      }   
   }

   /if (${Plugin[MQ2MoveUtils].Name.Length}==NULL) {
      /echo MQ2MoveUtils not loaded. Cannot continue. Ending.
      /end
   }
/RETURN
