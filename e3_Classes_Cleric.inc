
| CALLED WHENEVER A CLERIC CLASS STARTS E3
SUB CLR_Setup
/RETURN

| CALLED TO CREATE CLERIC BOT INI ENTRIES
SUB CLR_CharacterSettings
	/call WriteToIni "${iniFileCharacter},Buffs,Courage" "SelfOverMana:70"
	/call WriteToIni "${iniFileCharacter},Heals,Minor Healing" "HealPct:50"
/RETURN

| CREATE CLERIC ALIASES ON INITIAL SETUP
SUB CLR_Aliases

/RETURN


| ROUTINE CLERIC FUNCTIONS
SUB CLR_Background_Events

/RETURN



SUB CLR_createSpells
	/declare spells[100,10,100] string local

	/varset spells[65,1,${bName}] 				Supernal Light
	/varset spells[65,1,${bHealPct}] 			70
	/varset spells[65,1,${bMemInGem}] 			1
	/varset spells[65,2,${bName}] 				Supernal Remedy
	/varset spells[65,2,${bHealPct}] 			35
	/varset spells[65,2,${bMemInGem}] 			1
	/varset spells[65,3,${bName}]				Divine Arbitration
	/varset spells[65,3,${bHealPct}] 			20
	/varset spells[65,3,${bCancelAtHP}] 		90
	/varset spells[65,3,${bRequireGroup}] 		1
	/varset spells[65,3,${bNoCancelChecks}] 	1



	/declare i int local
	/declare j int local

	/for i 1 to 100
		/for j 1 to 10
			/if (${spells[${i},${j},${bName}].Length}==0) /continue
			/echo ${spells[${i},${j},${bName}]}
		/next j
	/next i

/RETURN