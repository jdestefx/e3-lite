Sub casting_Setup
	/declare idleCastingTimer timer outer
	/declare idleCastingTimerDelay int outer 40

/return

Sub casting_Background_Events
/return

SUB casting_MacroSettings
/RETURN

SUB casting_CharacterSettings
/RETURN

SUB e3_getCastTypeByCastRequest(string castRequest) 
   /if (${Me.AltAbility[${castRequest}].ID}) /return ALTABILITY
   /if (${Me.CombatAbility[${castRequest}]}) /return DISCIPLINE
   /if (${Spell[${castRequest}].ID}) /return SPELL
   /if (${FindItemCount[${castRequest}]}) /return ITEM
   /RETURN UNKNOWN
/RETURN

SUB e3_getSpellByCastRequestName(string castRequest) 
   /if (${FindItemCount[=${castRequest}]}>0) /return FindItem[${castRequest}].Spell
   /if (${Spell[${castRequest}].ID}) /return Spell[${castRequest}]
   /if (${Me.AltAbility[${castRequest}].ID}) /return Me.AltAbility[${castRequest}].Spell
   /if (${Me.CombatAbility[${castRequest}]}) { 
      /varset combatAbilityIndex ${Me.CombatAbility[${castRequest}]}
      /return CombatAbility[${combatAbilityIndex}]
   }

   /RETURN UNKNOWN
/RETURN

SUB e3_Cast(int targetID, string castName, string cancelConditions)
	/declare maxFizzle int local 3
	/declare fizzleCount int local 0
	/declare castType string local
	/declare castReagent string local NULL
	/declare castSpell spell local
	/declare memGemSlot int local
	/declare ret string local UNKNOWN
	/declare bardWasTwisting bool local FALSE
	/declare bardWasTwistingList string local
	/declare doCastingChecksAfterTime timer local 0
	/declare startPos string local ${Me.Y},${Me.X},${Me.Z}
	/declare precastWatchTimer timer local 
	/declare castItemName string local 
	/declare combatTimerValue int local

	| SILENCED? BAIL
	/if (${Me.Silenced.ID}!=NULL) /return SILENCED

	| RESOLVE A SPELL OBJECT BASED ON THE REQUESTED CAST NAME
	/call e3_getSpellByCastRequestName "${castName}"
	/if (${Macro.Return.Equal[UNKNOWN]}) /return UNKNOWN
	/vardata castSpell ${Macro.Return}

	| IF IM NOT THE TARGET SEE IF THE TARGET IS IN RANGE
	/if (${targetID} != NULL && ${targetID} != ${Me.ID} && ${castSpell.MyRange}>0 && ${Spawn[id ${targetID}].Distance3D} > ${castSpell.MyRange}) /return DISTANCE

	| SEE IF WE HAVE LINE OF SIGHT TO THE TARGET
	/if (${Spawn[id ${targetID}].LineOfSight}==FALSE && ${castSpell.SpellType.Find[Detri]}) /return LOS

	| DETERMINE THE SPELL ORIGIN (CAST TYPE) FOR FURTHER HANDLING
	/call e3_getCastTypeByCastRequest "${castName}"

	/if (${Macro.Return.Equal[UNKNOWN]}) /return UNKNOWN
	/varset castType ${Macro.Return}

	| IF THE SPELL IS NOT AN ITEM CLICK AND NEEDS MANA, SEE IF WE HAVE ENOUGH
	/if (${castType.NotEqual[ITEM]} && ${castSpell.Mana} > 0) {
		/if (${Math.Calc[${Me.CurrentMana}]} < ${castSpell.Mana}) /return OOM
	}

	/if (${castType.Equal[ITEM]}) {
		/varset castItemName ${FindItem[${castName}].Spell.Name}
	}

	/if (${castType.Equal[DISCIPLINE]}) {
		/if (${Me.CombatAbilityReady[${castName}]}==FALSE) /return NOTREADY
		/disc ${castName}
		/bc e3-info-cast: ${castName}(${Target})
		/return CAST_SUCCESS
	}

	/if (${castType.Equal[SPELL]} && ${Me.SpellReady[${castName}]}==FALSE) /return NOTREADY
	/if (${castType.Equal[ALTABILITY]} && ${Me.AltAbilityReady[${castName}]}==FALSE) /return NOTREADY
	/if (${castType.Equal[SPELL]} && ${Me.SpellReady[${castName}]}==FALSE && ${Me.Class.ShortName.NotEqual[BRD]}) /return COOLDOWN

	/if (${Me.Class.ShortName.Equal[BRD]}) {
		/call bardStopSinging
		/varset bardWasTwisting ${Macro.Return}
		/if (${bardWasTwisting}==TRUE) /varset bardWasTwistingList ${Twist.List}
	}

	:castStart

	| PRESERVE COMBAT TIMER VALUE SO IT DOESN'T EXPIRE JUST BECAUSE BOT WAS CASTING
	/varset combatTimerValue ${combatTimer}

	/if (${castSpell.TargetType.NotEqual[SELF]} && ${Target.ID} != ${targetID}) {
		/tar id ${targetID}
		/delay 1
	}

	/bc Casting [${castName}] => ${Target.Name}
	
	/if (${Me.Class.ShortName.Equal[BRD]} && ${castType.Equal[SPELL]}) {
		/twist ${Me.Gem[${castName}]}
	} else {
		/casting "${castName}"
	}

	/varset precastWatchTimer 10
	/while (${Me.Casting.ID}==NULL && ${precastWatchTimer}>0) {
		/delay 1
		/doevents
	}

	/while (${Cast.Status.Find[C]}!=NULL) {
		/if (${castType.Equal[ITEM]} && ${Cast.Effect.Name.NotEqual[${castItemName}]}) /break
		/doevents
		/delay 1

		/if (${castSpell.TargetType.NotEqual[SELF]} && ${Target.ID} != ${targetID}) {
			/if (${cancelConditions.Upper.Find[TARGET]}!=NULL) {
				/call fullTarget ${targetID}
			} else {
				/tar id ${targetID}
			}
		}

		/if (${assistTarget}==0 && ${Stick.Active}==TRUE) {
			/interrupt
			/break
		}

		/if (${Spawn[id ${targetID}].Dead}==TRUE) {
			/return CAST_TARGET_DEAD
		}

		/if (${Math.Distance[${startPos}]} > 2  && ${Select[${Me.Class.ShortName},BRD]}==0) {
			/bc I moved while casting; Interrupting. (Delta: ${Math.Distance[${startPos}]})
			/interrupt
			/break
		}

		/call checkCancelConditions "${cancelConditions}"
		/if (${Macro.Return}==TRUE) {
			/bc Cancel conditions met. (${cancelConditions})
			/interrupt
			/break
		}
	}

	/if (${Me.Class.ShortName.Equal[BRD]}) {
		/if (${bardWasTwisting}==TRUE) {
			/delay 3
			/twist ${bardWasTwistingList}
		} else {
			/twist off
		}
	}

	/varset idleCastingTimer ${idleCastingTimerDelay}

	| MANDATORY PAUSE HERE BECAUSE CAST.RESULT NEEDS TO DETECT THE OUTCOME
	/delay 10

	| RESET COMBATTIMER TO WHAT IT WAS BEFORE CAST START
	/if (${combatTimerValue}>0) /varset combatTimer ${combatDelay}

	/echo CR:  CastResult: ${Cast.Result}

	/if (${Cast.Result.Equal[CAST_FIZZLE]}) {
		/varset fizzleCount ${Math.Calc[${fizzleCount}+1]}
		/if (${fizzleCount} <= ${maxFizzle}) /goto :castStart
	}

	/if (${Select[${Cast.Result},CAST_RESIST,CAST_SUCCESS,CAST_INTERRUPTED,CAST_CANCELLED,CAST_IMMUNE]}>0) {
		/return ${Cast.Result}
	}

	/if (${Cast.Taken}==TRUE) /return CAST_NOTAKEHOLD

	/return CAST_UNKNOWN
/RETURN

SUB isCastReady(string request)
	/declare castType string local

	/call e3_getCastTypeByCastRequest "${request}"
	/varset castType ${Macro.Return}

	/if (${castType.Equal[UNKNOWN]}) /return FALSE

	/if (${castType.Equal[SPELL]} && ${Me.SpellReady[${request}]}==FALSE) /return FALSE
	/if (${castType.Equal[ALTABILITY]} && ${Me.AltAbilityReady[${request}]}==FALSE) /return FALSE
	/if (${castType.Equal[ITEM]} && ${Me.ItemReady[${request}]}==FALSE) /return FALSE
	/if (${castType.Equal[SPELL]} && ${Me.SpellInCooldown}==TRUE && ${Me.Class.ShortName.NotEqual[BRD]}) /return FALSE
	/if (${castType.Equal[DISCIPLINE]} && ${Me.CombatAbilityReady[${request}]}==FALSE) /return FALSE

	/return TRUE
/RETURN

SUB checkCancelConditions(string conditions)
	| RETURNING TRUE WILL TRIGGER THE WATCH LOOP TO BREAK

	/if (${conditions.Upper.Find[TARGETISASSITTARGET]}!=0) {
		/if (${assistTarget}==${Target.ID}) {
			/return TRUE
		}
	}

	/if (${conditions.Upper.Find[TARGETUNDERHP:]}!=0) {
		/call getEntryParam "${conditions}" TARGETUNDERHP
		/if (${Target.PctHPs} < ${Macro.Return}) {
			/return TRUE
		}
	}

	/if (${conditions.Upper.Find[TARGETLIFEPCT:]}!=0) {
		/call getEntryParam "${conditions}" TARGETLIFEPCT
		| TRYING NETBOTS TO ACCOUNT FOR THE LATENCY OF A HEALTH BAR WHEN CHANGING TARGETS
		/if (${NetBots[${Target.Name}].PctHPs} > ${Math.Calc[${Macro.Return}]}) {
			/return TRUE
		}
	}

	/if (${conditions.Upper.Find[TARGETHASBUFF:]}!=0) {
		/call getEntryParam "${conditions}" TARGETHASBUFF
		/if (${Target.Buff[${Macro.Return}].ID}!=NULL) {
			/return TRUE
		}
	}

	/IF (${conditions.Upper.Find[TARGETDEAD]}!=0) {
		/if (${Target.Dead}==TRUE || ${Target.ID}==NULL) {
			/return TRUE
		}
	}

	/if (${conditions.Upper.Find[NETBOTATTACKING]}!=0) { 
		/if (${isNetBotAttackingID[${Target.ID}]}==TRUE) {
			/return TRUE
		}
	}

	/return FALSE
/RETURN


SUB bardStopSinging
	/declare wasTwisting bool local ${Twist.Twisting}

	/if (${Twist}) /twist off
	/delay 5
	/interrupt

	/return ${wasTwisting}
/RETURN


SUB memSpell(int gemNumber, string spellName)
	/if (${gemNumber} < 1 || ${gemNumber} > 9) {
		/if (${Debug}) /echo [${gemNumber}] is out of bounds.
		/return ${spellName}_BADGEM
	} 

	/memspell ${gemNumber} "${spellName}"
	
	while (${Me.Gem[${gemNumber}].ID != ${Spell[${spellName}].ID}) {
		/delay 30
		/echo waiting to mem...
		/call Background_Events
	}
/RETURN

