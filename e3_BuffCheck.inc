
Sub buffs_Setup
   /declare acceptBuffRequests bool outer TRUE
   /declare buffQueue string outer
   /declare buffRequestRequiresGuildMatch bool outer TRUE
   /declare buffRequestMinimumMana int outer 50
   /declare buffRequestOOMEmoteTimer timer outer 0

   /declare buffRequestLockoutTimer timer outer 
   /declare buffsEnabled bool outer TRUE
   /declare buffCheckInterval int outer 20
   /declare buffCheckTimer timer outer 0
   /declare forceBuffs bool outer FALSE
   /declare needsBuffPlayerQueue list outer 
   /declare useStrictCombatCheck bool outer FALSE

   /call processBuffTargets

   /call getIniToVar "${iniFileCharacter},Misc,AcceptBuffRequests" acceptBuffRequests FALSE
   /call getIniToVar "${iniFileCharacter},Misc,BuffRequestMinimumMana" buffRequestMinimumMana 50
   /call getIniToVar "${iniFileCharacter},Misc,UseStrictCombatCheck" useStrictCombatCheck FALSE

   /if (${Defined[iniBuffRequests]}==NULL || ${iniBuffRequests[1,1].Equal[-]}) /varset acceptBuffRequests FALSE

/RETURN

SUB buffCheck_CharacterSettings(bool backFillOnly)
   /call WriteToIni "${iniFileCharacter},Misc,AcceptBuffRequests" NO ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Misc,BuffRequestMinimumMana" 50 ${backFillOnly}
   /call WriteToIni "${iniFileCharacter},Misc,UseStrictCombatCheck" NO ${backFillOnly}
/RETURN

SUB processBuffTargets(string arrayName)
   /if (${Defined[iniBuffs]}==FALSE) /return

   /declare i int local 1
   /declare j int local
   /declare finalTargets string local
   /declare removeTargets string local

   /for i 1 to 64
      /if (${iniBuffs[${i},1].Equal[-]}) /continue

      /if (${iniBuffs[${i},${bTargets}].NotEqual[-]}) {
         /call processTargets "${iniBuffs[${i},${bTargets}]}"
         /varset finalTargets ${Macro.Return}
      } else {
         /varset finalTargets ${allNetbotNames}
      }

      /if (${iniBuffs[${i},${bExcludeTargets}].NotEqual[-]}) {

         /call processTargets "${iniBuffs[${i},${bExcludeTargets}]}"
         /varset removeTargets ${Macro.Return}

         /for j 1 to ${Math.Calc[${removeTargets.Count[|]}+1]}
            /call removeToken "${finalTargets}" "${removeTargets.Token[${j},|]}" "|" "|"
            /varset finalTargets ${Macro.Return}
         /next j
      }

      /varset iniBuffs[${i},${bTargets}] ${finalTargets}

   /next i

/RETURN

Sub buffCheck_Background_Events
   /if (${acceptBuffRequests}==TRUE) {
      /call checkNeedsBuffsQueue
      /call checkBuffRequestQueue
   }

   /if (${buffsEnabled} && ${Defined[iniBuffs]}) {
      | CAN NEVER BUFF UNDER THESE CIRCUMSTANCES
      /if (${Me.Class.ShortName.NotEqual[BRD]} && ${Me.PctMana} < 10) {
         /if (${Select[${Me.Class.ShortName},${manaClasses}]} > 0) /return
      }

      /if (${Stick.Active}==TRUE && !${combatTimer}) /return
      /if (${Me.Invis}==TRUE && ${forceBuffs}==FALSE) /return

      | SOME MORE BAIL CONDITIONS
      /if (${Corpse.Open} || ${Me.Feigning}==TRUE) /return
      /if (${Me.Moving}==TRUE && ${canCastWhileMoving}==FALSE) /return
      /if (${buffCheckTimer} > 0 && ${forceBuffs}==FALSE) /return

      /varset buffCheckTimer ${buffCheckInterval}
      /call processBuffs
   }
/return

SUB checkNeedsBuffsQueue
   /if (${needsBuffPlayerQueue.Count}==0) /return
   /if (${buffQueue.Length}>0) /return

   /declare i int local
   /declare buffName string local
   /declare spawnID int local 
   /declare queueEntry string local
   /declare spellName string local
   /declare spawnName string local

   /varset spawnID ${Spawn[pc ${needsBuffPlayerQueue.Head[]}].ID}
   /varset spawnName ${Spawn[id ${spawnID}].CleanName}
   /tar id ${spawnID}

   /bc examining ${spawnName}
   /delay 1s

   /for i 1 to ${iniBuffRequests.Size}
      /if (${iniBuffRequests[${i},1].Equal[-]}) /break

      /varset spellName ${iniBuffRequests[${i},1]}
      /varset queueEntry ${spawnID}_${spellName}_${i}

      /if (${Select[EMOTABLE,${iniBuffRequests[${i},${bKeyword}]},]}==0) /continue

      /if (${iniBuffRequests[${i},${bRequireTargetClass}].NotEqual[-]}) {
         /if (${Select[${Spawn[id ${spawnID}].Class.ShortName},${iniBuffRequests[${i},${bRequireTargetClass}]}]}==0) /continue
      }

      /if (${Target.Buff[${spellName}].ID}==NULL) {
         /varset buffQueue ${queueEntry};${buffQueue}
      }

   /next i
/RETURN

SUB checkBuffRequestQueue
   /if (${buffRequestLockoutTimer}>0) /return

   /declare e string local
   /declare targetID int local
   /declare buffName string local
   /declare ei int local
   /declare i int local

   /if (${buffQueue.Length}==0) /return

   /if (${Me.PctMana} < ${buffRequestMinimumMana} && ${buffQueue.Length}>0) {
      /if (${buffRequestOOMEmoteTimer}==0) {
         /em is low on mana and will continue buff requests WMP.
         /varset buffRequestOOMEmoteTimer 60s
      }
      /return
   }

   /for i 1 to ${Math[${buffQueue.Count[;]}+1]}
      /varset e ${buffQueue.Token[${i},;]}
      
      /varset targetID ${e.Token[1,_]}
      /varset buffName ${e.Token[2,_]}
      /varset ei ${e.Token[3,_]}

      /if (${Spawn[pc id ${targetID}].ID}==NULL) {
         /varset buffQueue ${buffQueue.Replace[${e};,]}
         /return
      }

      /if (${iniBuffRequests[${ei},${bSelfOverMana}].NotEqual[-]}) {
         /if (${Me.PctMana} < ${iniBuffRequests[${ei},${bSelfOverMana}]}) /continue
      }

      /if (${iniBuffRequests[${ei},${bMemInGem}].NotEqual[-]}) {
         /if (${Me.Gem[${buffName}]}==NULL) {
            /call memorizeSpellInGem "${buffName}" ${iniBuffRequests[${ei},${bMemInGem}]}
         }
      }

      /call e3_Cast ${targetID} "${buffName}"
      
      /if (${Macro.Return.Equal[CAST_NOREADY]}) /continue

      /if (${Macro.Return.Equal[CAST_NOTAKEHOLD]} || ${Macro.Return.Equal[CAST_SUCCESS]} || ${Macro.Return.Equal[CAST_TAKEHOLD]} || ${Macro.Return.Equal[CAST_NOTTAKEHOLD]}) {
         /varset buffQueue ${buffQueue.Replace[${e};,]}
         /break
      }

   /next i
/RETURN

SUB processBuffs
   /declare i int local 
   /declare targetNames string local 
   /declare targetName string local
   /declare skipTargetProcessing bool local
   /declare buffCastName string local
   /declare buffSpell spell local
   /declare buffSpellID int local
   /declare netbotName string local
   /declare checkSpellID int local 
   /declare buffSpawn spawn local
   /declare noholdTimerName string local
   /declare buffSpellRange int local
   /declare cancelCheck string local
   /declare note string local 

   /declare j int local
   /declare k int local

   /for i 1 to ${iniBuffs.Size}
      /varset targetNames 
      /if (${iniBuffs[${i},${bTargets}].Length}==0) /continue
      /varset skipTargetProcessing FALSE

      /if (${iniBuffs[${i},1].Equal[-]}) /break

      | DONT BUFF IN COMBAT UNLESS ALLOWED
      /if (${useStrictCombatCheck}==TRUE && ${Me.CombatState.Equal[COMBAT]}) {
         /if (${iniBuffs[${i},${bCastableDuringAssist}].Equal[-]}) /continue
      }
      /if (${combatTimer}) {
         /if (${iniBuffs[${i},${bCastableDuringAssist}].Equal[-]}) /continue
      }

      /if (${iniBuffs[${i},${bNotIfAssistTag}].NotEqual[-]}) {
         /if (${assistTags.Count}>0 || ${persistentAssistTags.Count}>0) {
            /call validateAssistTags ${iniBuffs[${i},${bNotIfAssistTag}]} TRUE
            /if (${Macro.Return}==TRUE) /continue
         }
      }

      /if (${iniBuffs[${i},${bRequireAssistTag}].NotEqual[-]}) {
         /if (${assistTags.Count}==0 && ${persistentAssistTags.Count}==0) /continue
         /call validateAssistTags ${iniBuffs[${i},${bRequireAssistTag}]}
         /if (${Macro.Return}==FALSE) /continue
      }

      /if (${iniBuffs[${i},${bRequireCastIdle}].NotEqual[-]}) {
         /if (${idleCastingTimer}>0) /continue
      }
      /if (${iniBuffs[${i},${bRequireAssistTarget}].NotEqual[-]}) {
         /if (${assistTarget}==0) /continue
      }
      /if (${iniBuffs[${i},${bSelfOverHP}].NotEqual[-]}) {
         /if (${Me.PctHPs} < ${iniBuffs[${i},${bSelfOverHP}]}) /continue
      }
      /if (${iniBuffs[${i},${bSelfUnderHP}].NotEqual[-]}) {
         /if (${Me.PctHPs} >= ${iniBuffs[${i},${bSelfUnderHP}]}) /continue
      }
      /if (${iniBuffs[${i},${bSelfOverMana}].NotEqual[-]}) {
         /if (${Me.PctMana} < ${iniBuffs[${i},${bSelfOverMana}]}) /continue
      }
      /if (${iniBuffs[${i},${bSelfUnderMana}].NotEqual[-]}) {
         /if (${Me.PctMana} >= ${iniBuffs[${i},${bSelfOverMana}]}) /continue
      }
      /if (${iniBuffs[${i},${bCastableOnlyResting}].NotEqual[-]}) {
         /if (${Me.CombatState.Equal[COMBAT]}) /continue
      }
      /if (${iniBuffs[${i},${bCastOnlyDuringAssist}].NotEqual[-]}) {
         /if (${Me.CombatState.NotEqual[COMBAT]}) /continue
      }
      /if (${iniBuffs[${i},${bSelfUnderEnd}].NotEqual[-]}) {
         /if (${Me.PctEndurance} >= ${iniBuffs[${i},${bSelfUnderEnd}]} ) /continue
      }
      /if (${iniBuffs[${i},${bSelfOverEnd}].NotEqual[-]}) {
         /if (${Me.PctEndurance} < ${iniBuffs[${i},${bSelfUnderEnd}]} ) /continue
      }

      | OBTAIN BUFF SPELL DETAIL
      /varset buffCastName ${iniBuffs[${i},${bName}]}

      | RESOLVE THE CAST REQUEST INTO A SPELL
      /call e3_getSpellByCastRequestName "${buffCastName}"
      /if (${Macro.Return.Equal[UNKNOWN]}) /continue

      /vardata buffSpell ${Macro.Return}
      /varset buffSpellID ${buffSpell.ID}
      
      /if (${iniBuffs[${i},${bForceSpellID}].NotEqual[-]}) {
         /varset buffSpellID ${iniBuffs[${i},${bForceSpellID}]}
      }
      
      /if (${buffSpell.TargetType.Equal[Pet]} || ${iniBuffs[${i},${bTargetType}].Upper.Equal[PET]} ) {
         /if (${Me.Pet.ID}==NULL) /continue
         /if (${Me.PetBuff[${buffSpell.Name}]}!=NULL) /continue
         /varset targetNames ${Me.Pet.Name}|
         /goto :skipNetBots
      } else /if (${buffSpell.TargetType.Equal[Self]}) {
         /varset targetNames ${Me.CleanName}|
      } else {
         /varset targetNames ${iniBuffs[${i},${bTargets}]}|
      }

      /if (${buffSpell.TargetType.Equal[Group v1]}) /varset buffSpellRange ${buffSpell.AERange}
      /if (${buffSpell.TargetType.Equal[Group v2]}) /varset buffSpellRange ${buffSpell.AERange}
      /if (${buffSpell.TargetType.Equal[Single]}) /varset buffSpellRange ${buffSpell.MyRange}

      | TARGETNAMES ARE ALL POSSIBLE TARGETS REGARDLESS OF WHETHER OR NOT THEY HAVE THE BUFF.
      | THIS LOOP REMOVES PEOPLE WHO DONT QUALIFY FOR A CAST, TYPICALLY BECAUSE THEY HAVE THE
      | BUFF. BUFF ENTRIES WITH REBUFFSECONDS MAY KEEP PEOPLE IN THE LIST WHO ALREADY HAVE
      | THE BUFF IF THE TIME ON THE BUFF IS LOW ENOUGH.
      /for j ${targetNames.Count[|]} downto 1
         /varset targetName ${targetNames.Token[${j},|]}
         /if (${targetName.Length}==0) /continue


         /if (${Spawn[${targetName}].ID}==NULL) {
            /varset targetNames ${targetNames.Replace[${targetName}|,]}
            /continue
         }

         /if (${NetBots[${targetName}].State.Equal[DEAD]}) /continue

         | HAVE THE BUFF?
         /if (${Select[${buffSpellID},${NetBots[${targetName}].Buff}]}>0) {
            | REBUFF SECONDS DONT MATER? REMOVE THIS PERSON FROM BUFF CANDIDATES
            /if (${iniBuffs[${i},${bRebuffSeconds}].Equal[-]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }

            | REBUFFSECONDS ENTRY PRESENT, EXAMINE NETNOTE

            | EMPTY NETNOTE.  REMOVE FROM CANDIDATES SINCE THEY HAVE THE BUFF
            /if (${NetBots[${targetName}].Note.Length}==0) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }

            | NO NETNOTE FOR SPELL FOUND - ASSUME PERSON NEEDS BUFF - LEAVE THEM IN LIST

            /if (${NetBots[${targetName}].Note.Find[;${buffSpellID}_]}==NULL) /continue
            
            | NETNOTE FOR SPELLID EXISTS, CHECK
            /varset note ${NetBots[${targetName}].Note}
            /varset note ${note.Right[ -${note.Find[;${buffSpellID}_]} ]}

            | REMAINING BUFF TIME IS SUFFICIENT, REMOVE PLAYER FROM BUFF CANDIDATES
            /if (${note.Token[1,;].Token[2,_]} > ${iniBuffs[${i},${bRebuffSeconds}]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }

            | REMAINING TIME IS NOT SUFFICIENT, LEAVE THEM IN THE LIST BY DOING NOTHING
         }

         /if (${Select[${buffSpellID},${NetBots[${targetName}].ShortBuff}]}>0) {
            /varset targetNames ${targetNames.Replace[${targetName}|,]}
            /continue
         }

         /if (${NetBots[${targetName}].InZone}==FALSE) { 
            /varset targetNames ${targetNames.Replace[${targetName}|,]}
            /continue
         }

         /if (${iniBuffs[${i},${bExtendedStackCheck}].NotEqual[-]}) {
            /dquery ${targetName} -q "Spell[${buffSpell}].Stacks" 
            |/echo res: ${DanNet.Q}  ${buffSpell}
            /if (${DanNet.Q}==FALSE) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bRequireGroupInRange}].NotEqual[-]}) {
            /if (${targetName.NotEqual[${Me.CleanName}]} && ${Group.Member[${targetName}].ID}==NULL) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }

            /if (${Spawn[pc ${targetName}].Distance} > ${buffSpellRange}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bTargetUnderHP}].NotEqual[-]}) {
            /if (${NetBots[${targetName}].PctHPs} > ${iniBuffs[${i},${bTargetUnderHP}]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bTargetOverHP}].NotEqual[-]}) {
            /if (${NetBots[${targetName}].PctHPs} <= ${iniBuffs[${i},${bTargetOverHP}]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bIsShortBuff}].NotEqual[-]}) {
            |/if (${Select[${buffSpellID},${NetBots[${targetName}].ShortBuff}]}>0) {
            |   /varset targetNames ${targetNames.Replace[${targetName}|,]}               
            |   /continue
            |}
         }

         /if (${iniBuffs[${i},${bNotIfShortBuff}].NotEqual[-]}) {
            /varset checkSpellID ${Spell[${iniBuffs[${i},${bNotIfShortBuff}]}].ID}
            /if (${Select[${checkSpellID},${NetBots[${targetName}].ShortBuff}]}>0) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}               
               /continue
            }
         }

         /if (${iniBuffs[${i},${bNotIfLongBuff}].NotEqual[-]}) {
            /varset checkSpellID ${Spell[${iniBuffs[${i},${bNotIfLongBuff}]}].ID}
            /if (${Select[${checkSpellID},${NetBots[${targetName}].Buff}]}>0) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bTargetOverLevel}].NotEqual[-]}) {
            /if (${NetBots[${targetName}].Level} < ${iniBuffs[${i},${bTargetOverLevel}]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${iniBuffs[${i},${bTargetUnderLevel}].NotEqual[-]}) {
            /if (${NetBots[${targetName}].Level} >= ${iniBuffs[${i},${bTargetUnderLevel}]}) {
               /varset targetNames ${targetNames.Replace[${targetName}|,]}
               /continue
            }
         }

         /if (${Spawn[pc ${targetName}].Distance} > ${buffSpellRange}) {
            /varset targetNames ${targetNames.Replace[${targetName}|,]}
            /continue
         }
      /next j

      /if (${targetNames.Length}==0) {
         /if (${iniBuffs[${i},${bRevertToBandolier}].NotEqual[-]}) {
            /if (${bandolierProfileIsActive[${iniBuffs[${i},${bRevertToBandolier}]}]}==FALSE) {
               /band activate ${iniBuffs[${i},${bRevertToBandolier}]}
            }
         }
         /continue
      }

      /if (${iniBuffs[${i},${bActivateBandolier}].NotEqual[-]}) {
         /if (${bandolierProfileIsActive[${iniBuffs[${i},${bActivateBandolier}]}]}==FALSE) {
            /band activate ${iniBuffs[${i},${bActivateBandolier}]}
         }
      }

      /if (${iniBuffs[${i},${bRequireGroupInRange}].NotEqual[-]}) {
         /for k 1 to ${Group.Members}
            /if $({Group.Member[${k}].ID}==NULL) /continue
            /if (${Group.Member[${k}].Distance} > ${buffSpellRange}) /continue
         /next k
      }

      :skipNetBots

      /if (${iniBuffs[${i},${bUseHealIndex}].NotEqual[-]}) {
         /if (${targetNames.Count[|]}<${healIndex}) /continue
         /if (${Spawn[pc ${targetNames.Token[${healIndex},|]}].ID}==NULL) /continue
         /vardata buffSpawn Spawn[pc ${targetNames.Token[${healIndex},|]}]
      } else {
         /if (${iniBuffs[${i},${bTargetType}].Upper.Equal[PET]}) {
            /vardata buffSpawn Spawn[pcpet ${targetNames.Token[1,|]}]
         } else {
            /vardata buffSpawn Spawn[pc ${targetNames.Token[1,|]}]
         }
      }

      /if (${buffSpawn.ID}==NULL) /continue

      | NOW THAT WE HAVE THE SPELL RESOLVED, AND A SPAWN RESOLVED, BUILD A TIMER NAME, IF NEEDED LATER
      /varset noholdTimerName noHoldTimer_${buffSpawn.ID}_${buffSpellID}
      /if (${Defined[${noholdTimerName}]}==TRUE) {
         /if (${${noholdTimerName}}>0) /break
         /deletevar ${noholdTimerName}
      }

      /if (${iniBuffs[${i},${bNoCast}].NotEqual[-]}) /continue

      /if (${iniBuffs[${i},${bMemInGem}].NotEqual[-]}) {
         /if (${Me.Gem[${buffCastName}]}==NULL) {
            /call memorizeSpellInGem "${buffCastName}" ${iniBuffs[${i},${bMemInGem}]}
         }
      }

      | DIDNT FIND THE BUFF, CAST IT
      /call e3_Cast ${buffSpawn.ID} "${buffCastName}"

      /if (${Macro.Return.Equal[CAST_NOTAKEHOLD]}==TRUE) {
         /if (${Defined[${noholdTimerName}]}==TRUE) /deletevar ${noholdTimerName}
         /declare ${noholdTimerName} timer outer ${buffSpell.Duration}
      }

      /if (${Macro.Return.Equal[CAST_SUCCESS]}) {
         /if (${NetBots[${buffSpawn.CleanName}].Level}!=NULL) {
            /squelch /bct ${buffSpawn.CleanName} //netnote
         }
         /break
      }

      /if (${Macro.Return.Equal[CAST_CANCELLED]}) {
         /break
      }

   /next i

/RETURN

SUB getActiveBandolierName 
   /declare i int local

   /for i 1 to 8
      /if (${Me.Bandolier[${i}].Active}==TRUE) /return ${Me.Bandolier[${i}].Name}
   /next i

   /return NONE
/RETURN

SUB processTargets(string targets)
   /declare i int local 
   /declare e string local 
   /declare max int local ${Math.Calc[${targets.Count[,]}+1]}
   /declare ret string local
   /declare ret2 string local

   /for i 1 to ${max}
      /varset e ${targets.Token[${i},,]}
      /if (${e.Left[1].Equal[@]}) {
         /call getMatchesForKey "${e}"
         /if (${ret.Length}==0) {
            /varset ret ${Macro.Return}
         } else {
            /varset ret ${Macro.Return}|${ret}
         }
      } else {
         /if (${ret.Length}==0) {
            /varset ret ${e}
         } else {
            /varset ret ${ret}|${e}
         }
      }
   /next i

   /return ${ret}
/RETURN

SUB getMatchesForKey(string allowKey)
   /declare i int local 
   /declare netbotCount int local ${Math.Calc[${NetBots.Counts}]}
   /declare netbotName string local
   /declare netbotClass string local 
   /declare ret string local

   /for i 1 to ${netbotCount}
      /varset netbotName ${NetBots.Client[${i}]}
      /call characterPassesAllowKey ${netbotName} ${allowKey}
      /if (${Macro.Return}==TRUE) {
         /if (${ret.Length}==0) {
            /varset ret ${netbotName}
         } else {
            /varset ret ${ret}|${netbotName}
         }
      }
   /next i

   /return ${ret}
/RETURN

SUB characterPassesAllowKeySeries(string charName, string series)
   /declare i int local 0
   /declare ikey string local

   /for i 1 to ${Math.Calc[${series.Count[,]}+1]}
      /varset ikey ${series.Token[${i},,]}

      /if (${ikey.Left[1].Equal[@]}) {
         /call characterPassesAllowKey ${charName} ${ikey}
         /if (${Macro.Return}==TRUE) /return TRUE
      } else {
         /if (${charName.Upper.Equal[${ikey.Upper}]}) /return TRUE
      }
   /next i

   /return FALSE
/return

SUB characterPassesAllowKey(string charName, string allowKey) 
   /if (${Spawn[pc ${charName}].ID}==NULL) /return FALSE

   /declare searchKey string

   /declare xspawn spawn local
   /vardata xspawn Spawn[pc ${charName}]

   /if (${allowKey.Equal[ALL]}) /return TRUE
   /if (${allowKey.Upper.Equal[@ALLNETBOTS]}) /return TRUE

   /if (${allowKey.Upper.Equal[@MANAREGENCLASSES]}) {
      /if (${manaRegenClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@HASTECLASSES]}) {
      /if (${hasteClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@MANACLASSES]}) {
      /if (${manaClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@MELEECLASSES]}) {
      /if (${meleeClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@MONKS]}) {
      /if (${xspawn.Class.ShortName.Equal[MNK]}) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@KNIGHTS]}) {
      /if (${knightClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@TANKS]}) {
      /if (${tankClasses.Find[${xspawn.Class.ShortName}]}!=NULL) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@CLERICS]}) {
      /if (${xspawn.Class.ShortName.Equal[CLR]}) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Equal[@BARDS]}) {
      /if (${xspawn.Class.ShortName.Equal[BRD]}) /return TRUE
      /return FALSE
   }

   /if (${allowKey.Upper.Left[5].Equal["ONLY:"]}==TRUE) {
      /varset searchKey ,${allowKey.Arg[2,:]},
      /if (${searchKey.Upper.Find[${charName.Upper}]}!=0) /return TRUE
   }

   /if (${allowKey.Upper.Equal[@GROUP]}==TRUE) {
      /if (${Group.Member[${charName}]}!=NULL) /return true
   }
/RETURN FALSE

#EVENT buffsOff "#*#e3 buffs off#*#"
SUB event_buffsOff(line, ChatSender)
   /bc ${Me.Name} turning buffs off.
   /varset buffsEnabled FALSE
/RETURN

#EVENT buffsOn "#*#e3 buffs on#*#"
SUB event_buffsOn(line, ChatSender)
   /bc ${Me.Name} turning buffs on.
   /varset buffsEnabled TRUE
/RETURN

#EVENT healsOff "#*#e3 heals off#*#"
SUB event_healsOff(line, ChatSender)
   /bc ${Me.Name} turning heals off.
   /varset healsEnabled FALSE
/RETURN


#EVENT healsOn "#*#e3 heals on#*#"
SUB event_healsOn(line, ChatSender)
   /bc ${Me.Name} turning heals on.
   /varset healsEnabled TRUE
/RETURN


#EVENT newBotInBCC "#*#has joined the server#*#"
SUB event_newBotInBCC
   /call gatherNetbotNames
/RETURN

#Event tellsYou "#1# tells you, #2#" 
SUB EVENT_tellsYou(string line, string who, string msg)
   /if (${tellRelay.Equal[-]}) /return

   /declare finalCmd string local ${tellRelay.Lower}

   /varset finalCmd ${finalCmd.Replace[<who>,${who}]}
   /varset finalCmd ${finalCmd.Replace[<msg>,${msg}]}
   /docommand ${finalCmd}
/RETURN

#Event buffRequest "#*# tells you, '#2# #1#'" 
#Event buffRequest "#1# tells you, '#2#'" 
SUB EVENT_buffRequest(string line, string who, string msg)
   /declare i int local 0
   /declare requestKeyword ${msg.Upper.Replace[ ,]}
   /declare spawnID int local
   /declare spellName string local
   /declare queueEntry string local

   |/if (${combatTimer}>0) {
   |   /tell ${who} I'm too busy right now.
   |   /return
   |}

   /for i 1 to ${iniBuffRequests.Size}
      /if (${iniBuffRequests[${i},1].Equal[-]}) /break

      /if ( ${Select[${requestKeyword},${iniBuffRequests[${i},${bKeyword}]}]}!=0) {
         /varset spawnID ${Spawn[pc ${who}].ID}
         /varset spellName ${iniBuffRequests[${i},1]}
         /varset queueEntry ${spawnID}_${spellName}_${i}

         /if (${buffQueue.Find[${queueEntry}]}!=NULL) {
            /tell ${who} You're already queued to receive ${spellName}.
            /continue
         }

         /if (${iniBuffRequests[${i},${bRequireTargetClass}].NotEqual[-]}) {
            /if (${Select[${Spawn[id ${spawnID}].Class.ShortName},${iniBuffRequests[${i},${bRequireTargetClass}]}]}==0) {
               |/tell ${who} Sorry, I'm not allowed to cast ${spellName} on your class.
               /continue
            }
         }

         /varset buffQueue ${queueEntry};${buffQueue}
         /bc added ${who} to queue. ${buffQueue}
         /em nods at ${who}.
      }

   /next i
/RETURN

#Event pleaseBuffs "#*#says, '#1# needs buffs'" 
#Event pleaseBuffs "#*#says, '#1# needs buffs.'" 
#Event pleaseBuffs "#1# needs buffs." 
SUB EVENT_pleaseBuffs(string line, string emoter, string hailee)
   /if (${acceptBuffRequests}==FALSE) /return
   /if (${needsBuffPlayerQueue.Index[${emoter}]}>-1) /return
   /invoke ${needsBuffPlayerQueue.Append[${emoter}]}
   /em looks ${emoter} up and down.
   /delay 2
/RETURN


#Event hailResponse "#1# says, 'Hail, #2#'" 
SUB EVENT_hailResponse(string line, string hailer, string hailee)
   /if (${acceptBuffRequests}==FALSE) /return
   /if (${hailee.NotEqual[${Me.CleanName}]}) /return

   /declare response string local 
   /declare i int local 0
   /declare spewedSpells string local
   /declare cleanKeywords string local

   /varset response Send me a tell with one of the following requests:

   /for i 1 to ${iniBuffRequests.Size}
      /if (${spewedSpells.Find[${iniBuffRequests[${i},1]},]}!=NULL) /continue
      /if (${iniBuffRequests[${i},1].Equal[-]}) /break

      /varset cleanKeywords ${iniBuffRequests[${i},${bKeyword}].Upper.Replace[EMOTABLE,]}
      /if (${cleanKeywords.Right[1].Equal[,]}) /varset cleanKeywords ${cleanKeywords.Left[-1]}

      /varset response ${response} ... [ ${cleanKeywords} => ${iniBuffRequests[${i},1]} ]
      /varset spewedSpells ${iniBuffRequests[${i},1]},${spewedSpells}
   /next i

   /say Hail, ${hailer}. ${response} (${Me.PctMana}m).
/RETURN

