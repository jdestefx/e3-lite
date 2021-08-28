SUB heals_CharacterSettings(bool backFillOnly)
	/call WriteToIni "${iniFileCharacter},Misc,XTargetHealsEnabled" NO ${backFillOnly}
	/call WriteToIni "${iniFileCharacter},Misc,PanicHealSpell" NONE ${backFillOnly}
	/call WriteToIni "${iniFileCharacter},Misc,PanicHealLifePct" 50 ${backFillOnly}
	/call WriteToIni "${iniFileCharacter},Misc,AutoRez" NO ${backFillOnly}
	/call WriteToIni "${iniFileCharacter},Misc,AutoRezSpells" NONE ${backFillOnly}
	/call WriteToIni "${iniFileCharacter},Misc,AutoRezRequireHealIndex" NONE ${backFillOnly}
/RETURN

SUB heals_Setup
	/declare healsEnabled bool outer TRUE
	/declare healSpells[0] string outer
	/declare otherHealerNetBots string outer
	/declare healIndex int outer 1
	/declare enableHealIndex bool outer TRUE
	/declare checkHealerIndexTimer timer outer 0
	/declare healerCheckTimer timer outer 0
	/declare xTargetHealsEnabled bool outer FALSE

	/declare curseRemovals[0] string outer
	/declare poisonRemovals[0] string outer
	/declare doCurseRemoval bool outer TRUE
	/declare doPoisonRemoval bool outer TRUE

	/declare panicTargetID int outer 0
	/declare panicHealActive bool outer FALSE
	/declare panicHealSpellName string outer NONE
	/declare panicHealLifePct int outer 50
	/declare autoRez bool outer FALSE
	/declare autoRezSpells string outer NONE
	/declare autoRezRequireHealIndex int outer 0
	/declare checkAutoRezTimer timer outer 3s

	/call getIniToVar "${iniFileCharacter},Heal Team,OtherHealerNetBots" otherHealerNetBots NONE
	/call getIniToVar "${iniFileCharacter},Misc,XTargetHealsEnabled" xTargetHealsEnabled NO
	/call getIniToVar "${iniFileCharacter},Misc,PanicHealSpell" panicHealSpellName NONE
	/call getIniToVar "${iniFileCharacter},Misc,PanicHealLifePct" panicHealLifePct 50
	/call getIniToVar "${iniFileCharacter},Misc,AutoRez" autoRez NO
	/call getIniToVar "${iniFileCharacter},Misc,AutoRezSpells" autoRezSpells NONE
	/call getIniToVar "${iniFileCharacter},Misc,AutoRezRequireHealIndex" autoRezRequireHealIndex 0

	/call processHealTargets
/RETURN

SUB processHealTargets
	/if (${Defined[iniHeals]}==FALSE) /return

	/declare i int local 1
	/declare j int local
	/declare finalTargets string local
	/declare removeTargets string local

	/for i 1 to 64
		/if (${iniHeals[${i},1].Equal[-]}) /continue

		/if (${iniHeals[${i},${bTargets}].NotEqual[-]}) {
			/call processTargets "${iniHeals[${i},${bTargets}]}"
			/varset finalTargets ${Macro.Return}
		} else {
			/varset finalTargets ${allNetbotNames}
		}

		/if (${iniHeals[${i},${bExcludeTargets}].NotEqual[-]}) {
			/call processTargets "${iniHeals[${i},${bExcludeTargets}]}"
			/varset removeTargets ${Macro.Return}
			/for j 1 to ${Math.Calc[${removeTargets.Count[|]}+1]}
				/call removeToken "${finalTargets}" "${removeTargets.Token[${j},,]}" "|" "|"
				/varset finalTargets ${Macro.Return}
			/next j
		}

		| CODE ELSEWHERE REQUIRES HEALTARGETS TO BE CSV
		/varset iniHeals[${i},${bTargets}] ${finalTargets.Replace[|,,]} 

	/next i
/RETURN

SUB heal_Background_Events
	/if (${checkHealerIndexTimer}==0) /call determineHealerIndex
	
	/if (${panicHealActive}==TRUE) {
		/call checkPanicHeal
		/return
	}

	/if (${healsEnabled}==TRUE && ${Defined[iniHeals]}) {
		/call checkHeals
	}

	/if (${autoRez}==TRUE) {
		/call checkAutoRez	
	}
/RETURN

SUB checkAutoRez
	/if (${checkAutoRezTimer}>0) /return
	/varset checkAutoRezTimer 20

	/if (${Me.Moving}==TRUE) /RETURN
	/if (${Stick.Active}==TRUE) /RETURN
	/if (${Me.Silenced.ID}!=NULL) /RETURN
	/if (${Me.Invis}==TRUE) /RETURN

	/declare xSpawnID int local
	/declare corpseSearch string local pccorpse radius 150
	/declare useRezSpell string local
	/declare x int local
	/declare allcorpses[100] spawn local
	/declare corpseCount int local

	/if (${autoRezRequireHealIndex}!=0) {
		/if (${healIndex}<${autoRezRequireHealIndex}) /return
	}

	/vardata corpseCount SpawnCount[${corpseSearch}]
	/if (${corpseCount}==0) /return FALSE

	/for x 1 to ${corpseCount}
		/if (${isLockoutExpired[rez_${NearestSpawn[${x},${corpseSearch}].ID}]}==FALSE) {
			/continue
		} else {
			/varset xSpawnID ${NearestSpawn[${x},${corpseSearch}].ID}
		}
	/next x

	/if (${xSpawnID}==NULL) /return
	
	/call getReadyRezSpell

	/if (${Macro.Return.Equal[-]}) /return

	/bc AutoRez:  ${xSpawnID} ${Spawn[id ${xSpawnID}].Name}
	/call e3_cast ${xSpawnID} "${Macro.Return}"

	/echo create lockout: rez_${xSpawnID}
	/call lockout rez_${xSpawnID} 45s
/RETURN

SUB getReadyRezSpell
	/declare i int local 

	/if (${autoRezSpells.Find[,]}==0) {
		/if (${isCastReady[${autoRezSpells}]}==FALSE) {
			/return -
		} else {
			/return ${autoRezSpells}
		}
	} else {
		/for i 1 to ${Math.Calc[${autoRezSpells.Count[,]}+1]}
			/if (${isCastReady[${autoRezSpells.Token[${i},,]}]}==TRUE) {
				/return ${autoRezSpells.Token[${i},,]}
			}
		/next i
	}

	/return -
/RETURN

SUB checkPanicHeal 
	/if (${Me.Casting.ID}!=NULL) /return
	/if (${Me.SpellInCooldown}==TRUE) /return
	/if (${Stick.Active}==TRUE || ${Me.Moving}==TRUE) /return

	/declare waitTime int local

	/varset waitTime ${Math.Calc[ ${Spell[${panicHealSpellName}].CastTime} / 100 / 3]}
	/echo ${waitTime}

	/call fullTarget ${panicTargetID}
	/call e3_Cast ${panicTargetID} "${panicHealSpellName}" "TARGETLIFEPCT:${panicHealLifePct}" ${waitTime}
	
/RETURN

SUB check_CurseRemoval 
	/declare i int local 0
	/declare netbotName string local

	/for i 1 to ${netbotCount}
		/varset netbotName ${NetBots.Client[${i}]}

		/if (${NetBots[${netbotName}].Cursed}) {

		}
	/next i
/RETURN


SUB determineHealerIndex
	/if (${otherHealerNetBots.Equal[NONE]}) /return

	/if (${checkHealerIndexTimer}>0) /return
	/varset checkHealerIndexTimer 2s

	/declare i int local
	/declare j int local
	/declare entryCount int local ${Math.Calc[${otherHealerNetBots.Count[,]}+1]}
	/declare entryName string local
	/declare myManaAmount int local
	/declare entryManaAmount int local

	/declare highMana int local 101

	| RESET TO 1 FOR THIS SCAN
	/varset healIndex ${Math.Calc[${entryCount}+1]} 

	|/if (${Me.Stunned}==TRUE || ${Me.SpellInCooldown}==TRUE) {
	|   /varset healIndex 100
	|   /return
	|}

	/for i 1 to ${entryCount}
		/varset entryName ${otherHealerNetBots.Arg[${i},,]}
		/varset entryName ${entryName.Left[1]}${entryName.Right[-1].Lower}

		/varset myManaAmount ${Me.PctMana}
		/varset entryManaAmount ${NetBots[${entryName}].PctMana}

		| ARE THEY CASTING SOMETHING? BUMP ME UP
		|/if (${Bool[${NetBots[${entryName}].Casting}]}==TRUE) {
		|   /varset healIndex ${Math.Calc[${healIndex}-1]}
		|   /continue
		|}

		/if (${NetBots[${entryName}].InZone}==FALSE) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
			/continue
		}

		| IS ANOTHER NETBOT STUNNED OR DEAD? INCREASE MY PRIORITY
		/if (${Select[${NetBots[${entryName}].State},DEAD,STUN,HOVER]}>0) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
			/goto :loopend
		}

		| DOES MY MANA MATCH ANOTHER EXACTLY? ONE OF US NEEDS TO DEFER - USE ID AS A SEQUENTIAL DIFFERENTIATOR
		/if (${entryManaAmount}==${myManaAmount}) {
			| IF MY ID IS HIGHER THAN THEIR ID, INCREASE MY PRI
			/if (${Me.ID}>${Spawn[PC ${entryName}].ID}) {
				/varset healIndex ${Math.Calc[${healIndex}-1]}
			}
		} else /if (${myManaAmount} > ${entryManaAmount}) {
			/varset healIndex ${Math.Calc[${healIndex}-1]}
		}
		:loopend
	/next i
/RETURN

SUB checkHeals
	/if (${healerCheckTimer}>0) /RETURN
	/varset healerCheckTimer 5

	/if (${Me.Moving}==TRUE) /RETURN
	/if (${iniHeals.Size}==0) /RETURN
	/if (${Stick.Active}==TRUE && ${assistTarget}==0) /RETURN
	/if (${Me.Silenced.ID}!=NULL) /RETURN
	/if (${Me.Invis}==TRUE) /RETURN

	/declare i int local
	/declare cancelConditions string local
	/declare healEntry string local
	/declare healTargetName string local
	/declare healSpawn spawn local
	/declare healSpellName string local
	/declare healSpell spell local
	/declare healPct int local 90
	/declare useHealIndex bool local FALSE
	/declare checkSpellID int local
	/declare healLockoutTimerName string local
	/declare triggerIdleCasting bool local TRUE
	/declare cancelHealHP int local 

	/declare botBuffIDs string local 

	/for i 1 to ${iniHeals.Size}
		/varset triggerIdleCasting TRUE
		/varset botBuffIDs
		/varset healEntry ${iniHeals[${i},1]}
		/if (${healEntry.Equal[-]}) /goto :loopend

		/if (${iniHeals[${i},${bEnabled}].Equal[NO]}) /continue

		/varset healSpellName ${iniHeals[${i},1]}
		/varset healLockoutTimerName heal_${healSpawn.ID}_${healSpellName}

		/if (${iniHeals[${i},${bRequireCastIdle}].NotEqual[-]}) {
			/if (${idleCastingTimer}>0) /continue
		}

		/if (${iniHeals[${i},${bNotIfAssistTag}].NotEqual[-]}) {
			/if (${assistTags.Count}>0 || ${persistentAssistTags.Count}>0) {
				/call validateAssistTags ${iniHeals[${i},${bNotIfAssistTag}]} TRUE
				/if (${Macro.Return}==TRUE) /continue
			}
		}

		/if (${iniHeals[${i},${bRequireAssistTag}].NotEqual[-]}) {
			/if (${assistTags.Count}==0 && ${persistentAssistTags.Count}==0) /continue
			/call validateAssistTags ${iniHeals[${i},${bRequireAssistTag}]}
			/if (${Macro.Return}==FALSE) /continue
		}

		/if (${iniHeals[${i},${bNotIfMounted}].NotEqual[-]}) {
			/if (${Me.Mount.ID}!=NULL) /continue
		}

		/if (${iniHeals[${i},${bCastableOnlyResting}].NotEqual[-]}) {
			/if (${Me.CombatState.Equal[COMBAT]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfOverHP}].NotEqual[-]}) {
			/if (${Me.PctHPs} < ${iniHeals[${i},${bSelfOverHP}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfUnderHP}].NotEqual[-]}) {
			/if (${Me.PctHPs} >= ${iniHeals[${i},${bSelfUnderHP}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfOverMana}].NotEqual[-]}) {
			/if (${Me.PctMana} < ${iniHeals[${i},${bSelfOverMana}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bSelfUnderMana}].NotEqual[-]}) {
			/if (${Me.PctMana} >= ${iniHeals[${i},${bSelfUnderMana}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bRequireMobsInProximity}].NotEqual[-]}) {
			/if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 40]} < ${iniHeals[${i},${bRequireMobsInProximity}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bRequireMaxMobsInProximity}].NotEqual[-]}) {
			/if (${SpawnCount[npc loc ${Me.X} ${Me.Y} radius 75]} > ${iniHeals[${i},${bRequireMaxMobsInProximity}]}) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bRequireHurtPartyMembers}].NotEqual[-]}) {
			/if (${checkHurtPartyMembers[${iniHeals[${i},${bRequireHurtPartyMembers}]},45]}==FALSE) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bUseHealIndex}].NotEqual[-]}) {
			/varset useHealIndex TRUE
		} else {
			/varset useHealIndex FALSE
		}

		/if (${iniHeals[${i},${bHealPct}].NotEqual[-]}) /varset healPct ${iniHeals[${i},${bHealPct}]}

		/if (${isRealSpell[${healSpellName}]}==FALSE) {
			/beep
			/echo >>> Warning: "${healSpellName}" valid sanity checks. Bad spelling? Not a real spell? Run out of item?
			/echo >>> Disabling "${healSpellName}" heal entry. Restart E3 to re-enable.
         	/varset iniHeals[${i},${bEnabled}] NO
			/continue
		}

		/call e3_getSpellByCastRequestName "${healSpellName}"
		/vardata healSpell ${Macro.Return}

		/if (${iniHeals[${i},${bTargetType}].Upper.Equal[PET]} || ${healSpell.TargetType.Equal[Pet]} && ${Me.Pet.ID}!=NULL ) {
			/vardata healSpawn Spawn[id ${Me.Pet.ID}]
		} else {
			/call getNetBotReadyForHeal ${healPct} ${i}
			/if (${Macro.Return.Equal[NONE]}) /goto :loopnexti		

			/if (${useHealIndex}==TRUE && ${enableHealIndex}==TRUE && ${otherHealerNetBots.NotEqual[NONE]}) {
				/varset healTargetName ${Macro.Return}
				/if (${healTargetName.Count[;]} < ${healIndex}) /goto :loopnexti
				/varset healTargetName ${healTargetName.Arg[${healIndex},;]}
			} else {
				/varset healTargetName ${Macro.Return}
				/varset healTargetName ${healTargetName.Arg[1,;]}
			}

			/if (${healSpell.TargetType.Equal[Self]}) /varset healTargetName ${Me.CleanName}
			/vardata healSpawn Spawn[=${healTargetName}]
		}

		/if (${iniHeals[${i},${bRecastDelay}].NotEqual[-]}) {
			/if (${isLockoutExpired[heal_${healSpawn.ID}_${healSpell.ID}]}==FALSE) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bMemInGem}].NotEqual[-]}) {
			/if (${Me.Gem[${healEntry}]}==NULL) {
				/call memorizeSpellInGem "${healEntry}" ${iniHeals[${i},${bMemInGem}]}
			}
		}

		/if (${isCastReady[${healSpellName}]}==FALSE) {
			/goto :loopnexti
		}

		/if (${iniHeals[${i},${bDoCommand}].NotEqual[-]}) /docommand ${iniHeals[${i},${bDoCommand}]}


		/if (${healSpawn.Distance3D} > ${healSpell.MyRange} && ${healSpawn.Distance3D} > ${healSpell.AERange}) /goto :loopnexti
		|/if (${healSpawn.Distance3D} > ${healSpell.MyRange}) /goto :loopnexti

		/if (${iniHeals[${i},${bNotIfShortBuff}].NotEqual[-]}) {
			/varset checkSpellID ${Spell[${iniHeals[${i},${bNotIfShortBuff}]}].ID}
			/if (${Select[${checkSpellID},${NetBots[${healSpawn.Name}].ShortBuff}]}>0) {
				/goto :loopnexti
			}
		}

		| CHECK SHORTBUFFS
		/if (${iniHeals[${i},${bCheckShortBuffs}].NotEqual[-]}) {
			/varset botBuffIDs ${NetBots[${healSpawn.Name}].ShortBuff}
			/if (${botBuffIDs.Find[${healSpell.ID}]}!=0) /goto :loopnexti
		}
		/if (${iniHeals[${i},${bCheckLongBuffs}].NotEqual[-]}) {
			/varset botBuffIDs ${NetBots[${healSpawn.Name}].ShortBuff}
			/if (${botBuffIDs.Find[${healSpell.ID}]}!=0) /goto :loopnexti
		}

		/if (${iniHeals[${i},${bNoCancelChecks}].Equal[-]}) {
			/varset cancelHealHP ${iniHeals[${i},${bCancelAtHP}]}
			/if (${cancelHealHP}==0) /varset cancelHealHP ${healPct}
			/varset cancelConditions TARGETLIFEPCT:${cancelHealHP}|TARGETHASBUFF:${healSpell.Name}
		} else {
			/varset cancelConditions TARGETHASBUFF:${healSpell.Name}
		}


		/if (${iniHeals[${i},${bRequireIdleCasting}].NotEqual[-]}) {
			/if (${idleCastingTimer}>0) /return
			/varset triggerIdleCasting FALSE
		}

		/if (${assistTarget}!=0) /stick off

		/call e3_Cast ${healSpawn.ID} "${healEntry}" "${cancelConditions}" ${triggerIdleCasting}

		/if (${Macro.Return.Equal[CAST_SUCCESS]}) {
			/if (${iniHeals[${i},${bRecastDelay}].NotEqual[-]}) {
				/varset healLockoutTimerName heal_${healSpawn.ID}_${healSpell.ID}
				/call lockout ${healLockoutTimerName} ${iniHeals[${i},${bRecastDelay}]}
			}

			/if (${iniHeals[${i},${bNoSitTimer}].NotEqual[-]}) /call pauseMedBreak ${iniHeals[${i},${bNoSitTimer}]}
			
			/return TRUE
		} else /if (${Macro.Return.Equal[NOTREADY]}) {
			/goto :loopnexti
		} else {
			/echo CastResult Unhandled: ${Macro.Return}
			/return
		}

		:loopnexti
	/next i
	:loopend

	/return FALSE

/RETURN

SUB checkHurtPartyMembers(string params, int distanceLimit)
	/declare requireHurtChars int local ${params.Token[1,@]}
	/declare requireHealthPct int local ${params.Token[2,@]}
	/declare hurtChars int local 0

	/if (${Me.Grouped}==FALSE) /return FALSE

	/declare i int local 0
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
		/if (${Group.Member[${i}].Dead}==TRUE) /continue
		/if (${distanceLimit} > 0 && ${Group.Member[${i}].Distance} > ${distanceLimit}) /continue
		/if (${Group.Member[${i}].PctHPs} > ${requireHealthPct}) /continue

		/varset hurtChars ${Math.Calc[${hurtChars}+1]}
	/next i

	/if (${hurtChars}>= ${requireHurtChars}) /return TRUE

	/return FALSE

/RETURN

SUB getNetBotReadyForHeal(int healPct, int iniHealIndex)
	/declare i int local 0
	/declare netbotName string local
	/declare netbotClass string local
	/declare netbotCount int local ${NetBots.Counts}
	/declare lowestPct int local 100
	/declare healNames list local
	/declare li listiterator local
	/declare b string local
	
	/if (${xTargetHealsEnabled}==TRUE && (${iniHeals[${iniHealIndex},${bXTargetHeal}].NotEqual[-]} || ${iniHeals[${iniHealIndex},${bXTargetOnlyHeal}].NotEqual[-]}) )  {
		/for i 1 to 12
			/if (${Me.XTarget[${i}].ID}==NULL) /continue
			/if (${Me.XTarget[${i}].ID}==0) /continue
			/if (${Me.XTarget[${i}].Type.NotEqual[PC]} && ${Me.XTarget[${i}].Type.NotEqual[Pet]}) /continue
			/if (${Me.XTarget[${i}].Type.Equal[Pet]} && ${Me.XTarget[${i}].Master.Type.NotEqual[PC]}) /continue
			/if (${Me.XTarget[${i}].PctHPs} <= ${healPct}) {
				/if (${healNames.Index[${Me.XTarget[${i}].CleanName}]}!=-1) /continue
				/if (${isLockoutExpired[heal_${Me.XTarget[${i}].ID}_${iniHeals[${iniHealIndex},1]}]}==FALSE) /continue

				/if (${Me.XTarget[${i}].PctHPs}<${lowestPct}) {
					/varset lowestPct ${Me.XTarget[${i}].PctHPs}
					/invoke ${healNames.Insert[0,${Me.XTarget[${i}].CleanName}]}
				} else {
					/invoke ${healNames.Append[${Me.XTarget[${i}].CleanName}]}
				}
			}
		/next i
	}

	/if (${iniHeals[${iniHealIndex},${bXTargetOnlyHeal}].NotEqual[-]}) /goto :doneWithNames

	/if (${Me.Grouped}==TRUE && ${iniHeals[${iniHealIndex},${bIncludeGroup}].NotEqual[-]}) {
		/for i 1 to 5
			/if (${Group.Member[${i}].ID}==NULL) /continue
			/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
			/if (${Group.Member[${i}].OtherZone}==TRUE) /continue
			/if (${Group.Member[${i}].PctHPs} <= ${healPct}) {
				/if (${healNames.Index[${Group.Member[${i}]}!=-1) /continue				
				/if (${isLockoutExpired[heal_${Group.Member[${i}].ID}_${iniHeals[${iniHealIndex},1]}]}==FALSE) /continue
				/if (${Group.Member[${i}].PctHPs}<${lowestPct}) {
					/varset lowestPct ${Group.Member[${i}].PctHPs}
					/invoke ${healNames.Insert[0,${Group.Member[${i}]}]}
				} else {
					/invoke ${healNames.Append[${Group.Member[${i}]}]}
				}
			}
		/next i
	}


	/for i 1 to ${netbotCount}
		/varset netbotName ${NetBots.Client[${i}]}
		/if (${netbotName.Equal[NULL]}) /break


		/varset netbotClass ${NetBots[${netbotName}].Class.ShortName}

		/if (${NetBots[${netbotName}].InZone} == FALSE) /continue

		/if (${Spawn[${netbotName}].Dead}==TRUE) /continue

		/if (${NetBots[${netbotName}].State.Equal[DEAD]}==TRUE || ${NetBots[${netbotName}].State.Equal[STUN]} && ${NetBots[${netbotName}].CurrentHPs} < -1) /continue

		/if (${NetBots[${netbotName}].Invulnerable}==1) /continue

		/if (${iniHeals[${iniHealIndex},${bTargets}].NotEqual[-]}) {
			/if (${Select[${netbotName},${iniHeals[${iniHealIndex},${bTargets}]}]}==0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bCheckShortBuffs}].NotEqual[-]}) {
			/call e3_getSpellByCastRequestName "${iniHeals[${iniHealIndex},1]}"
			/if (${Macro.Return.Equal[UNKNOWN]}) /continue
			/if (${NetBots[${netbotName}].ShortBuff.Find[${${Macro.Return}.ID}]}!=0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireGroup}].NotEqual[-]}) {
			/if (${Group.Member[${netbotName}].ID}==NULL) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bTargetUnderLevel}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Level} >= ${iniHeals[${iniHealIndex},${bTargetUnderLevel}]}) /continue
		}
		/if (${iniHeals[${iniHealIndex},${bTargetOverLevel}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Level} <= ${iniHeals[${iniHealIndex},${bTargetOverLevel}]}) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireDiseased}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Diseased} == 0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequirePoisoned}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Poisoned} == 0) /continue
		}

		/if (${iniHeals[${iniHealIndex},${bRequireCursed}].NotEqual[-]}) {
			/if (${NetBots[${netbotName}].Cursed} == 0) /continue
		}

		| bRequireHurtPartyMembers implies group memeber, so verify
		/if (${iniHeals[${iniHealIndex},${bRequireHurtPartyMembers}].NotEqual[-]}) {
			/if (${Group.Member[${netbotName}].ID}==NULL) /continue
		}

		/if (${NetBots[${netbotName}].PctHPs} <= ${healPct}) {
			/if (${healNames.Index[${netbotName}]}!=-1) /continue
			/if (${NetBots[${netbotName}].PctHPs} < ${lowestPct}) {
				/varset lowestPct ${NetBots[${netbotName}].PctHPs}
				/invoke ${healNames.Insert[0,${netbotName}]}
			} else {
				/invoke ${healNames.Append[${netbotName}]}
			}
		}

	/next i

	:doneWithNames

	/if (${healNames.Count}==0) /return NONE

	/vardata li healNames.First
	/while (${li.IsEnd}==FALSE) {
		/varset b ${b}${li.Value}
		/invoke ${li.Advance}
		/if (${li.IsEnd}==TRUE) /break
		/varset b ${b};
	}

	/return ${b};
/RETURN

#EVENT xthModeChange "#*#e3 xth #1#"
SUB event_xthModeChange(string line, string xthMode)
	/if (${xthMode.Upper.Equal[ON]}) {
		/varset xTargetHealsEnabled TRUE
	} else /if (${xthMode.Upper.Equal[OFF]}) {
		/varset xTargetHealsEnabled FALSE
	} else {
		/bc Unknown XTH mode. Use on or off.
		/return
	}

	/bc XTargetHeals turned ${xthMode}.
/RETURN


#EVENT panicHeal "#*#e3 panicheal #1#"
SUB event_panicHeal(string line, string panicParam)

	/if (${panicParam.Upper.Equal[OFF]}) {
		/varset panicHealActive FALSE
		/bc Panic healing OFF.
	} else {
		/if (${panicHealSpellName.Upper.Equal[NONE]}) {
			/bc I have no panic heal spell defined in my INI. Cannot use panic heal.
			/return
		}

		/if (${Spawn[pc ${panicParam}].ID}==NULL) {
			/bc Unknown Panic Heal Target ${panicParam}
			/return
		}

		/varset panicTargetID ${Spawn[pc ${panicParam}].ID}
		/varset panicHealActive TRUE

		/bc New panic heal target: ${Spawn[pc ${panicParam}].CleanName}
	}
/RETURN


#EVENT autoRezToggle "#*#e3 autorez #1#"
SUB event_autoRezToggle(string line, string newMode)
	/if (${newMode.Upper.Equal[ON]}) {
		/varset autoRez TRUE
	} else /if (${newMode.Upper.Equal[OFF]}) {
		/varset autoRez FALSE
	} else {
		/bc Unknown AutoRez mode. Use ON or OFF.
		/return
	}

	/bc AutoRez is now: ${newMode}.

/RETURN