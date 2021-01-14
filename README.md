# e3-lite
e3-lite is an adaptation of the original "killians e3 macro".  

To use, just run /mac e3.  The first time you execute the macro, a basic INI will be created.  Below explains all the areas and what you can do with them.

	======================
	INI SECTIONS
	======================
	[Misc]

	"EQBC Channels" (Space separated words)
	Channels to be automatically assigned to upon macro start.

	"Tell Relay" (command)
	Command to execute when a tell is received.  <who> and <msg> are replaced with actual values.

	"AcceptBuffRequests" (YES/NO)
	Enable or disable buff requests.

	"BuffRequestMinimumMana" (INT)
	Global mana requirement for fulfilling buff requests.


	[Assist]

	AutoDisarm (YES/NO)  ***WIP***
	Looks for traps within 75 units and attemps to automatically use "Disarm Traps" on it.  Requires hard-coded traps names in 
	e3_Assists.inc assist_Setup function to support trap names. 

	Auto-Assist (YES/NO)
	Monitors other characters' combat activity and controls whether or not to adopt their target as the character's own assist target.

	Melee (YES/NO)
	Use melee-combat as a form of assisting.

	Stick Point (BEHIND, FRONT, ETC -- MQ2MOVEUTILS /STICK PARAMETERS)
	Where to stick to a mob when melee-assisting. Typically this will always be "behind" unless the character is a tank.

	Melee Distance (INT)
	Stick distance value.  If you /bcaa //varset assistStickDistance N, characters will automatically adjust to the new value within
	a few seconds.  Recommended: /alias /adist /bca //varset assistStickDistance

	Slay Index (INT)
	Slay is a mechanic that will automatically set assist targets when there are multiple mobs on your group.
	When slay mode is on, this determines which add to offtank. If you have 4 tanks, each one would be 1,2,3,4, etc.
	Leave at 99 for all other characters you don't want participating in slay mechanics.

	SlayReferencePoint (SELF NAME)
	Leave this as yourself (each toon should have their own name here).

	BeneficialStrip (ITEM/SPELL NAME)
	Originally intended for monk-oow-robe clicking, if you have an item that can be used to dispell beneficial buffs 
	from an assist target, you can put it here.

	Tank (YES/NO)
	Whether or not this character should behave as a tank.  Used in conjunction with [AggroAbilities] section. This is
	a global on/off for tank logic.

	DontTakeAggroFrom (COMMA SEPARATED NAMES)
	A list of names of people to not take aggro from when tank logic is on.

	MaxAssistDistance (INT)
	Distance at which assist calls will be ignored.

	OnSlayAssistCommand (COMMAND)
	A command to execute when a character shifts onto a slay target.  "/bc debuffs on !!{Target.ID}" is a useful one.
	Notice !! instead of $ for dynamic commands. 

	IgnoreAssistCalls (YES/NO)
	Provides a simple global mechanism to make a character ignore assist calls.  Allows you to use a simple "/bc assist on ${Target.ID}"
	button and not have to find ways to exclude certain characters from engaging, like clerics.

	MaintainAssistLOS=NO
	If los is lost during assist, attempt to /nav to the target.

	[Buffs]

	Buffs are spells that will be casted on any character in your eqbcs server and uses mq2netbots to determining readiness. Buffs
	are only casted out of combat unless otherwise specified using the combat-related flags below.

	Example entry:
	Visions of Grandeur=SelfOverMana:25|Targets:@Monks,Penrilgone|TargetOverLevel:45|CastableDuringAssist
		
	If you want to have multiple entries for the same spell, you need to add #1, #2, etc to the end of the spell name:

	;This prioritizes giving monks haste, then only giving Penrilgone haste as extra mana permits.
	Visions of Grandeur#1=SelfOverMana:25|Targets:@Monks
	Visions of Grandeur#2=SelfOverMana:95|Targets:Penrilgone
			
	Supported Flags:
	---------------------------------------------------------------------------------------------------------------------
	CastableDuringAssist	Lets the buff be casted even if the buffer has an active assist target.
	CastOnlyDuringAssist	Can only be cast during assist. 
	Targets 				CSVs of @Class or character names.
	SelfOverMana			Buffer must be at or over this percent mana.
	SelfUnderMana			Buffer must be under this percent mana.
	TargetOverLevel			Target must be at or over this level for this spell.
	NoCancelChecks			This flag will ignore whether or not the target character already has the buff being cast.
					Netbots has a weird thing where a buff will appear to be "gone" from a character despite 
					being on it's last few seconds. So, when buffer begins to cast the buff, they will immediately
					notice the buff is still on the character, and stop casting.  This cycle repeats 4 or 5 
					times until the buff is actually gone. You will probably want this on most buff entries.
	RequireGroup 			The target must be in the buffer's group.
	RequireGroupInRange		All group memebers must be in range before this spell will cast.
	SelfOverHP			Buffer must be at or over this much HP. Good for shaman canni.
	SelfUnderHP			Buffer must be at under this much HP.
	CastableOnlyResting		Buff can only be cast while not assisting.
	SelfUnderEnd			Buffer must be under this percent endurance.
	SelfOverEnd			Buffer must be at or over this percent endurance.
	ForceSpellID			For odd scenarios (Heroic Bond) where there are multiple spellIDs for the same spell name,
					You sometimes need to force a spell ID to prevent chain-casting of a spell.
	TargetUnderHP			Target must be under this percent HP.
	TargetOverHP			Target must be at or over this percent HP.
	NotIfShortBuff			Don't cast if the target has this effect in their song window.
	NotIfLongBuff			Don't cast if the target has this effect in their buff window.
	TargetUnderLevel		Target must be under this level.
	TargetOverLevel			Target must be at or over this level.


	[Debuffs]

	Debuffs are spells that will be casted when a character sees "debuffs on [spawnID]".  Unlike AssistSpells, debuffs can be
	called on multiple targets.

	Similar to buffs, the structure is the same: Spell=Flags.

	; Mana drain
	Theft of Thought=SelfUnderMana:75|RequireTargetClass:CLR,DRU,SHM,NEC,WIZ,MAG,ENC,SHD,PAL


	Supported flags:
	-----------------------------------------------------------------------------------------
	SelfOverMana				Buffer must be over this percent mana.
	SelfUnderMana				Buffer must be under this percent mana.
	SelfUnderEnd				Buffer must be under this percent endurance.
	SelfOverEnd				Buffer must be at or over this percent endurance.
	TargetOverHP				Target must be at or over this percent life.
	TargetUnderHP				Target must be under this percent life.
	TargetUnderLevel			Target must be under this level.
	TargetOverLevel				Target must be at or over this level.
	RequireBodyType				Primarily used to restrict undead nukes to "UNDEAD" type.
	TargetRace				Restrict to certain races.
	RequireMobsInProximity			Require this many mobs to be in a small radius. Used for rains or PBAEs.
	RequireMaxMobsInProximity		Prevents casting if there are more than N mobs nearby. Example usage (1) might be to allow clerics 
						to cast nukes, but only if the group is fighting one at a time.
	RequireTargetClass 			CSV list of class abbreviations.  Good for restricting mana drains to only caster classes.
	RequireNoActiveDisc			Require that there is currently no disc running.  This is good for chaining discs.
	NoSitTimer				Prevents sitting (if medidate is on) for this amount of time after a successful cast.
						Can be several formats: 50 = 5 seconds.  5s = 5 seconds.
	RecastDelay 				Adds a delay between casting this spell entry as to not drain mana too fast. Use deci-seconds only.
						ie.  50 = 5 seconds.  Does not support "5s" syntax.


	[AssistSpells]
	These are the spells that will be casted on the singular assist target, triggered when a character sees "assist on [spawnID]"
	These spells are processed using the same function as debuffs, so the exact same table of parameters above will apply.


	[BuffRequests]

	Allows certain buffs to be requested via /tell.  Keywords are not case-sensitive but will be printed verbatim when the buffer
	received a hail.

	Example entries:

	Visions of Grandeur#1=Keyword:Haste|SelfOverMana:50|RequireTargetClass:MNK,WAR,ROG
	Visions of Grandeur#2=Keyword:Haste|SelfOverMana:90|RequireTargetClass:PAL,SHD,BRD
	Gift of Pure Thought=Keyword:Mana|SelfOverMana:30|RequireTargetClass:CLR,DRU,SHM,NEC,WIZ,MAG,ENC,SHD,PAL
	Group Resist Magic=Keyword:GMR|SelfOverMana:30


	[Heals]

	Heals will evaluate all characters in your eqbcs server and uses mq2netbots for determining readiness.  Heals is also how you will
	configure poison and disease cures.  Keep in mind that heals are processed in order and once a heal is performed, the process 
	restarts at the top of the stack.

	Example enties:
	Word of Restoration=HealPct:100|RequireHurtPartyMembers:3@70|NoCancelChecks|NoSitTimer:100
	Divine Light#1=HealPct:80|SelfOverMana:90|NoSitTimer:100|UseHealIndex
	Divine Light#2=HealPct:70|SelfOverMana:30|NoSitTimer:100|UseHealIndex
	Divine Light#3=HealPct:30|NoSitTimer:100|NoCancelChecks|UseHealIndex
	Celestial Elixir=HealPct:60|SelfOverMana:40|UseHealIndex|CheckShortBuffs|NotIfShortBuff:Elixir of Healing IX
	Antidote=HealPct:100|RequirePoisoned|RequireMaxMobsInProximity:2|UseHealIndex

	Supported flags:
	---------------------------------------------------------------------------------------------
	HealPct					The primary indicator of when to cast this heal.
	Targets 				Restrict the heal to only these targets.
	RequireGroup 				Heal target must be in the caster's group.
	TargetUnderLevel			Target must be under this level.
	TargetOverLevel				Target must be at or over this level.
	RequireDiseased				Target must have a disease on them.
	RequirePoisoned				Target must have a poison on them.
	SelfOverHP				Caster must be at or over this much HP. Good for shaman canni.
	SelfUnderHP				Caster must be at under this much HP.
	SelfOverMana				Caster must be over this percent mana.
	SelfUnderMana				Caster must be under this percent mana.
	RequireMaxMobsInProximity		Prevents casting if there are more than N mobs nearby.
	RequireHurtPartyMembers			x@y format. X number of party members must be at or below y percent life.
	UseHealIndex				Used in conjunction with [Heal Team] to avoid redundant healing.
	DoCommand				Perform this ad-hoc command before casting the heal. See below for example.
	NotIfShortBuff				Don't cast the heal if this effect is in the song window.
	CheckShortBuffs				Some heals have a buff effect.  This instructs the heal routine to look in the song window. This is
						important for heal-over-time spells, otherwise the HOT will just repeatedly cast.
	CheckLongBuffs				Some heals have a buff effect.  This instructs the heal rountine to look in the main buff window for
						the heal spell.
	NoCancelChecks				By default, if the heal target goes above the HealPct threshhold while casting, the heal will
						automatically interrupt itself.  This can prevent that.  Good for heal entries aimed at tanks where
						damage might be so heavy that it's worth risking the overhealing, or if the HealPct is low enough
						that overhealing is very unlikely.
	NoSitTimer				Prevent sitting (if meditate is on) for this amount of time after a successful cast.


	Additional: 

	Here is a slightly mroe complicated example of how to get a monk to use Mend via the Heals system.
	Mend=HealPct:75|Targets:Damidhob|DoCommand:/doability "mend"


	[Heal Team]

	Heal team is used to form a team of healers who will try to avoid healing the same target at the same time, resulting in overhealing.

	Form your team by adding a line like the following, excluding the character who's INI you're editing.  Repeat this for all members of the
	heal team.

	OtherHealerNetBots=Pinzarn,Naltron


	[AutoAbilities]

	AutoAbilities controls actions located in your "Actions" window.  (Taunt, Bash, Kick, etc).  Most of these will require the RequireAssistTarget
	flag, otherwise the ability will trigger continuously.  You could use this to train feign death, theoretically.

	Flying Kick=RequireAssistTarget
	Tiger Claw=RequireAssistTarget

	The only supported flags for this section are: SelfUnderHP and RequireAssistTarget.
	TODO: Try "Mend=SelfUnderHP:50|RequireAssistTarget"

	[Burns]

	Burns are spells (or discs) that will trigger when /burn on # is activated.  Remember order matters. When you /burn on 60 (burn for 60 seconds), 
	You will first trigger Innerflame, then Thunderkick will continuously wait until there is no active disc, then Thunderkick will trigger.
	/burn on 5 is an easy way to just use whatever discipline is ready.  Casters can put spells in here, too.  All the paramters under [AssistSpells]
	are supported in this section.  Casters might have entries here that don't restrict mana as much, or have lower/no RecastTimer flags.

	Innerflame Discipline=RequireNoActiveDisc
	Thunderkick Discipline=RequireNoActiveDisc


	======================
	E3 COMMANDS
	======================

	The following are commands that trigger certain e3 behaviors for any character that sees the command.  What that means is that you can issue
	these commands to /bc (for everyone) or /bct someChannel to restrict the audience. This is why in the [Misc] section, there is an "EQBCS Channels"
	section. It's good practice to organize your crew into appropriate channels, then use /bct to command only those characters.

	For example, make all monks report an item:  "/bct monks e3 item 10"

	NOTE: Most commands start with "e3" but some do not.  Those are just old commands I haven't updated.  Just be cognizant of that when making your
	hotkeys.


	/bc e3 mmv
	/bc e3 dropinvis
	Removes any invisibility effect.

	/bc e3 aa [on|off]
	/bc e3 autoassist [on|off]
	Toggles auto-assist on|off.

	/bc e3 bs [on|off]
	/bc e3 beneficialStrip [on|off]
	Toggles beneficial strip logic [on|off].

	/bc e3 uhi [on|off]
	/bc e3 usehealindex [on|off]
	Toggles [Heal Team] logic [on|off]

	/bc e3 item [itemSlotID]
	Makes a character report Name, HP, Mana, and AC of an item in a provided [itemSlotID]

	/bc e3 buffs
	Triggers a temporary state where buffs will be cast regardless of combat state.  Old and may not work.

	/bc e3 all melee
	Enables melee combat as an assist type for any character who sees this command.  Good for making casters temporarily enable melee combat to get
	faction tags.

	/bc e3 burns on [time]
	Activates "burn" state, which triggers processing of [Burns] section in the INI. Supply a time either in deci-seconds (50) or abbreviated form (5s).

	/bc e3 burns off
	Deactivates burn state.

	/bc e3 slay [on|off]
	Toggles slay mode.

	/bc assist off
	Removes current assist target.  This is essentially your "stop attacking this thing" command.

	/bc assist on [spawnID]
	Begins attacking the specified spawnID

	/bc e3 debuffs off
	Removes all debuff targets from processing.  

	/bc e3 debuffs on [spawnID]
	Adds the spawnID to the list of debuff targets.

	/bc e3 roam with [spawnSearch]   (spawnSearch is usually just a name)
	Activates roam behavior.  Roam will keep characters near the "roam with" person.  As that person walks away or moves, the rest of the crew will 
	automatically follow.

	/bc e3 roam off
	Disables roam behaviors.

	/bc medbreak [on|off]
	Toggles medbreak behavior.  Medbreak will attempt to sit and regain mana unless medbreak has been paused by other mechanisms.

	/bc e3 buffs [on|off]
	Toggles buffing on|off.  Good for dispell fights where rebuffing will just cause problems.  Sometimes good after rezzing to just chill 
	for a bit until you're ready to do some buffing.

	/bc e3 heals [on|off]
	Toggles all heals on|off.  Good for somefights where you may want to manually control healing.  For example some AE fights where you want
	to cast trigger group heals manually on an as-needed basis.






