|------------------------------------------------------------------------|
|- e3_Macro_Loot.inc v2.1 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|${MacroQuest.ChatChannel[Creamo]}
|${MacroQuest.ChatChannel[1]}
|--------------------------------------------------------------------|
#event lootRelay "#1# tells you, 'loot #2#'"
#event lootRelay "<#1#> loot #2#"
#event lootRelay "[#1#(msg)] loot #2#"
SUB Event_lootRelay(line, ChatSender, looting)
  /if (${Me.Name.Equal[${ChatSender}]}) /return
  /docommand ${ChatToggle} Looting Turned ${looting}
  /call check_Loot "${looting}""

/return

Sub check_Loot(looting)

 /if (!${Defined[Auto_Loot]}) /declare Auto_Loot bool outer TRUE
  /if (!${Defined[LootOnly]}) /declare LootOnly bool outer FALSE
  /if (!${Defined[LootOnlyItem]}) /declare LootOnlyItem string outer
  
  /if (!${Defined[LootOnlyStackableEnabled]}) {
    /declare LootOnlyStackableEnabled bool outer
    /varset LootOnlyStackableEnabled False
  }
  /if (!${Defined[LootOnlyStackableValueGreater]}) {
    /declare LootOnlyStackableValueGreater int outer
    /varset LootOnlyStackableValueGreater 0
  }
  /if (!${Defined[LootOnlyStackableTradeSkillsCommon]}) {
    /declare LootOnlyStackableTradeSkillsCommon bool outer
    /varset LootOnlyStackableTradeSkillsCommon False
  } 
   /if (!${Defined[LootOnlyStackableTradeSkillsALL]}) {
    /declare LootOnlyStackableTradeSkillsALL bool outer
    /varset LootOnlyStackableTradeSkillsALL False
  } 

  /if (${Select[${looting},on]}) {
    /varset Auto_Loot TRUE
    /varset LootOnly FALSE
  } else /if (${Select[${looting},off]}) {
    /varset Auto_Loot FALSE
    |exit
    /return
  } else {
    /varset Auto_Loot TRUE
    /varset LootOnly TRUE
    /if (${Select[${looting},dc]}) {
      /varset looting Diamond Coin
    }
    /varset LootOnlyItem ${looting}
  }
  /if (!((${Me.Combat} || ${Me.CombatState.Equal[Combat]} ||  ${AssistTarget} >0 )) || ${combatLooting}) /call loot_Area

/return
|----------------------------------------------------------------|
|- Adds corpses that are not yours to the looted corpse list.	  -|
#EVENT NotYourKill "Someone is already looting that corpse."
#EVENT NotYourKill "You may not loot this corpse at this time."
#EVENT NotYourKill "You are too far away to loot that corpse."
SUB EVENT_NotYourKill
  /varset canLootCorpse FALSE
/RETURN

|--------------------------------------------------------------------------|
|- Moves to corpses and begins looting, reads Loot_Ini for loot handles.	-|
SUB loot_Area
  /declare NumCorpses int local ${SpawnCount[npc corpse zradius 50 radius ${seek_Radius} "_"]}
  /if (${NumCorpses}<1) {
    /return
  }
  /if (${NetBots[Creamo].ID}) /varset Debug_Loot TRUE
  /if (${Debug} || ${Debug_Loot}) /echo |- loot_Area ==>
  /declare startX int ${Me.X}
  /declare startY int ${Me.Y}
  
  |/if (${Twist.Twisting}) /call pauseTwist
  |/delay 5s !${Me.Casting.ID}
  /if (${AdvPath.Following}) /squelch /afollow off
  /if (${Stick.Active}) /squelch /stick off

  /declare ClosestCorpse        int local
  /declare ClosestCorpseDist    int local
  /declare TMPClosestCorpse      int local
  /declare TMPClosestCorpseDist  int local
  /declare CorpseList         string local
  /declare CorpseList_Looted  string local
  /declare i int local
  /declare c int local
 
  /declare CorpsesLeft int local ${NumCorpses}
  /varset cantLootCorpseCount 0
  /if (${Debug} || ${Debug_Loot}) /echo NumCorpses: ${NumCorpses}
  |/doevents flush YourKill
  /for i 1 to ${NumCorpses}
 /if (${NumCorpses}==0) /break
    /varset ClosestCorpse ${NearestSpawn[${i},npc corpse zradius 50 radius ${seek_Radius} "_"].ID}
    /varset CorpseList ${CorpseList}${ClosestCorpse}|
  /next i
  /if (${Debug} || ${Debug_Loot}) /echo CorpseList: ${CorpseList}
  /if (${NumCorpses}>0) {
    /squelch /hidecor looted
  }
  /for i 1 to ${NumCorpses}
 /if (${NumCorpses}==0) /break
    /if (${Debug} || ${Debug_Loot}) /echo Corpse ${i} of ${NumCorpses}, ${CorpsesLeft} left
    |iterate list of corpses
    /if (!${CorpsesLeft}) /return
    |recalculate the closest corpse from current position
    /varset ClosestCorpse ${CorpseList.Arg[1,|]}
    /varset ClosestCorpseDist ${Math.Distance[${Spawn[id ${ClosestCorpse}].Y},${Spawn[id ${ClosestCorpse}].X}]}
    /for c 1 to ${CorpsesLeft}
 /if (${CorpsesLeft}==0) /break
      /varset TMPClosestCorpse ${CorpseList.Arg[${c},|]}
      /varset TMPClosestCorpseDist ${Math.Distance[${Spawn[id ${TMPClosestCorpse}].Y},${Spawn[id ${TMPClosestCorpse}].X}]}
      |/if (${TMPClosestCorpseDist} < ${ClosestCorpseDist}) {
      |  /if (${Debug} || ${Debug_Loot}) /echo [${TMPClosestCorpse}]:${TMPClosestCorpseDist} closer than [${ClosestCorpse}]:${ClosestCorpseDist}
      |}
    /next c
    /if (!${Bool[${Spawn[${ClosestCorpse}]}]}) /goto :skipCorpse

    /if (${Target.ID}!=${ClosestCorpse}) /call TrueTarget ${ClosestCorpse}
    /delay 2s ${Target.ID}==${ClosestCorpse}
    /if (!${Target.ID}) {
      /if (${Debug} || ${Debug_Loot}) /echo Cant Target ${ClosestCorpse}
      /goto :skipCorpse
    }
    /if (${Target.Distance}>6 && ${Target.DistanceZ} < 80) {
      /call MoveToTarget 20 8
      |/call MoveToLoc ${Target.Y} ${Target.X} 20 12
    }
    /if (${Target.Distance}<=20 &&  ${Target.DistanceZ} < 40) {
      /call lootCorpse
    } else {
      /echo Corpse ${ClosestCorpse} is > ${Target.Distance}|${Target.DistanceZ} distance, skipping
    }
    :skipCorpse
    /varset CorpseList ${CorpseList.Replace[${ClosestCorpse}|,]}
    /varcalc CorpsesLeft ${CorpsesLeft}-1
    |exits loop if autoloot was turned off
    /doevents lootRelay
    /if (!${Auto_Loot}) /break
  /next i
  |if all corpses in radius cant be looted, dont try again until a new kill occurs
  /if (${cantLootCorpseCount}>=${SpawnCount[npc corpse zradius 50 radius ${seek_Radius} "_"]}) /doevents flush YourKill

  /if (${Following}) {
    /call AcquireFollow
  } else {
    /call MoveToLoc ${startY} ${startX} 15 20
  }
  |/squelch /hidecor npc 
  /docommand ${ChatToggle} ${Group.Leader} Looting Complete
  /if (${Debug} || ${Debug_Loot}) /echo <== loot_Area -|
/return

|-------------------------------------------------------------------------------------------------
SUB lootCorpse
  /declare i int local
  /declare j int local
  /declare stackCounter int local 
  /declare freeInventory int Local ${Me.FreeInventory}
  /declare itemName string local
  /declare itemSlot int local
  /varset canLootCorpse TRUE
  /declare importantItem bool local
 

  /loot
  /delay 5 ${Window[LootWnd].Open}
  /doevents NotYourKill
  /delay 1s ${Corpse.Items} || !${canLootCorpse}
  |so we found one item, now need to let the corpse fully fill out
  /delay 3 
  /declare numItems int local ${Corpse.Items}
  /if (${Debug} || ${Debug_Loot}) {
    /echo ===========================
    /echo Looting ${Target.ID}-Items:${Int[${numItems}]} - ${Time}
  }
  /if (!${canLootCorpse}) {
    /if (${Debug} || ${Debug_Loot}) /echo Not your kill
    /varcalc cantLootCorpseCount ${cantLootCorpseCount}+1
    /goto :skipLoot
  }
  /if (!${Bool[${Corpse.Items}]}) {
    |/if (${Debug} || ${Debug_Loot}) /echo Ending loot on ${Target.ID}, no items left: ${Corpse.Items}
    /goto :skipLoot
  }

  |must use numItems, ${Corpse.Items} recalcs every iteration
  /for i 1 to ${numItems}
 /if (${numItems}==0) /break
    |recalculate the free inventory
    /varcalc freeInventory ${Me.FreeInventory}-${LootNumOfFreeSlotsOpen}
    /if (${freeInventory}<1) {
      /varset freeInventory 0
    }
    |/if (${Debug} || ${Debug_Loot}) /echo starting loot i: ${i} of ${numItems}
    /if (${freeInventory}<1) {
      /if (!${fullInventory_Alert}) {
        /beep
        /varset fullInventory_Alert TRUE
        /docommand ${ChatToggle} My inventory is full!  I will continue to link items on corpses, but cannot loot anything else.
      }
      /if (${Debug} || ${Debug_Loot}) /echo no free inventory slots
    }

    /delay 1s ${Corpse.Item[${i}].ID}
    |/if (${Debug} || ${Debug_Loot}) /echo itemID ${Corpse.Item[${i}].ID}
  
    /if (${LootOnly}) {
      /if (${Corpse.Item[${i}].Name.Equal[${LootOnlyItem}]}) {
        /varset lootSetting Keep
      } else {
        /varset lootSetting Skip
      }
    } else /if (${LootOnlyStackableEnabled}) {

      |Stackable check code      
      /if (${Corpse.Item[${i}].Stackable} && !${Corpse.Item[${i}].NoDrop}) {

        /echo [${Time}]: Loot Only Stackable: value of item=${Corpse.Item[${i}].Value} vs setting=${LootOnlyStackableValueGreater}
        
        /varset lootSetting Skip
        /varset importantItem FALSE

        /if (${Defined[stackableItemArray]}) {
          /varset stackCounter 1
          /for stackCounter 1 to ${stackableItemArray.Size}
 /if (${stackableItemArray.Size}==0) /break
              /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[${stackableItemArray[${stackCounter}]}]}) {
                /varset importantItem TRUE
                /echo Found Important stackable item: ${stackableItemArray[${stackCounter}]}
                /goto :stackItemCheckComplete
              }
          /next stackCounter
          :stackItemCheckComplete
        }
       

        |items that are important, stackable but may have no vendor value
        |**
        /if (${Corpse.Item[${i}].Name.Equal[Lucky Coin]}) {
            /varset importantItem TRUE
        }
        /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[Perfected Augmentation Distiller]}) {
            /varset importantItem TRUE
        }
        /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[Diamond Coin]}) {
            /varset importantItem TRUE
        }
        /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[A Lucky Ticket]}) {
            /varset importantItem TRUE
        }
        /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[Mysterious Card]}) {
            /varset importantItem TRUE
        }
         /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[The Lady Luck blessings]}) {
            /varset importantItem TRUE
        }
        /if (!${importantItem} && ${Corpse.Item[${i}].Name.Equal[Silicorrosive Grease]}) {
            /varset importantItem TRUE
        }
        **|
        /if (${importantItem}) {

          /echo [${Time}]: Loot Only Stackable:hard coded important item ${Corpse.Item[${i}].Name}
        }
        |end items that are tackable but have no vendor value.

        |Loot tradeskill items if configured to do so.
        
        /if (!${importantItem}) {

            /if (${Corpse.Item[${i}].Value} >=${LootOnlyStackableValueGreater} ) {
              /varset importantItem True
              /echo [${Time}]: Loot Only Stackable:Keeping item ${Corpse.Item[${i}].Name} as its value is above or equal to threshhold
            } 
        }
        
        /if (!${importantItem} && ${LootOnlyStackableTradeSkillsALL} && ${Corpse.Item[${i}].Tradeskills}) {

          |We loot all trade skills 
            /varset importantItem TRUE
            /echo [${Time}]: Loot Only Stackable:Keeping item ${Corpse.Item[${i}].Name} as its a tradeskill item and we are set to loot all tradeskills.
        } 
        |/echo common trades skill var setting = ${LootOnlyStackableTradeSkillsCommon}
        /if (!${importantItem} && ${LootOnlyStackableTradeSkillsCommon}) {

          /if (!${importantItem} && ${Corpse.Item[${i}].Name.Find[ Pelt]}>0) {
            /varset importantItem TRUE
           }
          /if (!${importantItem} && ${Corpse.Item[${i}].Name.Find[ Silk]}>0) {
              /varset importantItem TRUE
          }
          /if (!${importantItem} && ${Corpse.Item[${i}].Name.Find[ Ore]}>0) {
              /varset importantItem TRUE
          }

          /if (${importantItem}) {
            /echo [${Time}]: Loot Only Stackable:Keeping item ${Corpse.Item[${i}].Name} as its a common tradeskill item.
          }
          
        }

        /if (${importantItem}) {
          /varset lootSetting Keep
        }

      } else {
        |not stackable, but stackable setting set so simply skip
        /varset lootSetting Skip
      }
      
      
    } else {
      |Default loot ini check
      /call get_lootSetting ${i} 0
    }

    /if (${lootSetting.Find[Skip]}) /goto :skipItem
    /varset itemName ${Corpse.Item[${i}].Name}
    /if (${Debug} || ${Debug_Loot}) /bc [Auto-Loot]: ${itemName}

    | Destroy the item
    |/if (${Debug} || ${Debug_Loot}) /echo loot setting: ${Corpse.Item[${i}].Name}-${lootSetting}
    /if (${lootSetting.Find[Destroy]}) {
      /echo [Loot]: Destroying [${Corpse.Item[${i}].Name}]
      /if (${Corpse.Item[${i}].Lore} && ${FindItemCount[=${Corpse.Item[${i}].Name}]}) /goto :skipLoot
      /call loot_Handle ${i} destroy
      | Keep the item
    } else /if (${lootSetting.Find[Keep]}) {
      | Check for a max stock
      /call argueString keep| "${lootSetting}"
      /if (${Bool[${c_argueString}]} && ${Int[${c_argueString}]} < ${Math.Calc[${FindItemCount[${Corpse.Item[${i}].Name}]} + 1].Int}) {
        /echo [Loot]: Fully stocked on [${Corpse.Item[${i}].Name}].
        /goto :skipItem
      }
      | If the item is lore, make sure I don't have one.  If I do, skip it.
      /if (${Corpse.Item[${i}].Lore}) {
          /if (${FindItemCount[=${Corpse.Item[${i}].Name}]} || ${FindItemBankCount[=${Corpse.Item[${i}].Name}]}) {
          /echo [Loot]: [${Corpse.Item[${i}].Name}] is lore, and I already have one${If[${FindItemBankCount[=${Corpse.Item[${i}].Name}]}, in the bank,]}.
          /goto :skipItem
        }
      }
      | Check that I have an inventory space large enough for the item
      | If
      | 1) No open slots
      | 2) The item on the corpse is stackable
      | 3) I have one of the stackable items in my inventory already
      | Then check to see if there is room within the stack to loot it.
      /if (${freeInventory}>1) {

        |we have some free inventory slots, lets see if any of them can contain the item we wish
        /if (${Me.LargestFreeInventory} < ${Corpse.Item[${i}].Size}) { 
            /echo [${Time}]: Loot: I don't have a free inventory space large enough to hold [${itemName}].
            /goto :skipItem
        }

      } else /if (${freeInventory}<1 && ${Corpse.Item[${i}].Stackable} && ${Bool[${FindItem[=${itemName}]}]}) {

        /if (${Debug} || ${Debug_Loot}) /bc [Auto-Loot]: Check stackables

        /if (!${Bool[${FindItem[=${itemName}].ItemSlot}]}) /goto :skipItem
        /varset itemSlot ${FindItem[=${itemName}].ItemSlot}
        /if (${Debug} || ${Debug_Loot}) /bc [Auto-Loot]: itemSlot: ${itemSlot}

        /if (${Bool[${Me.Inventory[${itemSlot}].Items}]}) {
          /for j 1 to 10
            /if (${Me.Inventory[${itemSlot}].Item[${j}].Name.Equal[${itemName}]}) {
              /if (!${Me.Inventory[${itemSlot}].Item[${j}].FreeStack} ) {
                /if (${Debug} || ${Debug_Loot}) /echo [Auto-Loot]: I've got a stack, but its full, skipping
                /goto :skipItem
              } else {
                /if (${Debug} || ${Debug_Loot}) /bc [Auto-Loot]: I've got a stack, but its NOT full, I should loot this
              }
            }
          /next j
        } else {
          /goto :skipItem
        }
      } else /if (${freeInventory}<1) {
        /if (${Debug} || ${Debug_Loot}) /bc [+g+] [Auto-Loot]: No Free Space
        /goto :skipItem
      }
      |/if (${Debug} || ${Debug_Loot}) /bc [+g+] [Auto-Loot]: Looting [${itemName}] keep
      
      |if we are keeping the item simply right click it for **far** faster looting.
      |delay 4 is needed per item so we don't accidently skip. Unsure how much delay will be impacted. 
      |if it is, uncomment out loot_handle and commonent out the itemnotify and /delay 4
      /itemnotify loot${i} rightmouseup
      /delay 2

      /if (${Window[QuantityWnd].Open}) {
          /notify QuantityWnd QTYW_Accept_Button leftmouseup
      }
      /while (${Bool[${Corpse.Item[${i}]}]} && ${Window[LootWnd].Open}) {
          |if item still on corpse, try again.
          |/echo trying to loot again ${i}
          /itemnotify loot${i} rightmouseup
          /delay 3
          /if (${Window[QuantityWnd].Open}) {
            /notify QuantityWnd QTYW_Accept_Button leftmouseup
            /delay 3
          }
      }
      |/call loot_Handle ${i} keep
      |/if (${Debug} || ${Debug_Loot}) /echo Done Looting i: ${i}
     
    }
  :skipItem

  |/if (${Debug} || ${Debug_Loot}) /echo Done with slot i: ${i}
  /next i
  |/delay 1
  |looting done, if items leftover link them
  |if numitems was 0, no reason to query the corpse again.
  /if (${linkChannel.Length}>0 && ${numItems}>0) {
    /if (${Corpse.Items}) {
      /keypress /
      /delay 1
      /call Type "${linkChannel} ${Target.ID}-"
      /delay 1
      /notify LootWnd BroadcastButton leftmouseup
      /delay 1
      /keypress enter chat
    }
  }
  :skipLoot
  /if (${Corpse.Open}) {
    /keypress esc
    |/notify LootWnd DoneButton leftmouseup
  } 
  /delay 1s !${Window[LootWnd].Open}
  |/if (${Debug} || ${Debug_Loot}) /echo Done Looting ${Time}
/return

|--------------------------------------------------------------------------|
SUB get_lootSetting(invSlot, itemSlot)
|/if (${Debug} || ${Debug_Loot}) /echo |- get_lootSetting invSlot: ${invSlot} ${itemSlot}
  /if (!${Bool[${Corpse.Items}]} && !${Window[MerchantWnd]}) /return
	/declare itemName string local
	/declare itemValue string local
  /declare itemRawValue int local
	/declare iniEntryVariables string local
	/varset lootSetting
	| If the item is not in a bag
	/if (!${Bool[${itemSlot}]}) {
		|/echo invSlot variable: ${invSlot} 
		| Reset itemName if it contains a ':'.  ':'s cause errors when reading from the ini, because they act as a delimiter, just like '='s
|		/echo ${Corpse.Item[${invSlot}].Name}		
		/varset itemName ${Corpse.Item[${invSlot}].Name}
		/if (${itemName.Find[:]}) /varset itemName ${itemName.Replace[:,;]}
    /if (${itemName.Find[,]}) {
      /call RemoveComma "${itemName}"
      /varset itemName ${Macro.Return}
    }
		| Set item value
		/varset itemValue ${Corpse.Item[${invSlot}].Value}
    	/varset itemRawValue ${Corpse.Item[${invSlot}].Value}
    	/varset itemValue ${If[${Bool[${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}]},${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}p,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}g,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}s,]}${If[${Bool[${itemValue.Right[1]}]},${itemValue.Right[1]}c,]}
		
		| Set ini variables like stack size, (C), (ND) etc.
		/varset iniEntryVariables ${If[${Corpse.Item[${invSlot}].Stackable},(${Corpse.Item[${invSlot}].StackSize}),]}${If[${Corpse.Item[${invSlot}].NoDrop},(ND),]}${If[${Corpse.Item[${invSlot}].Lore},(L),]}${If[${Corpse.Item[${invSlot}].Container},(C),]}
		| Check for a Loot_Ini entry
		|/echo /if (!{Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length})
		/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length}) /echo ifsuccess
		/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length}) {
			/echo /call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${Corpse.Item[${invSlot}].Container},Container,${If[${Corpse.Item[${invSlot}].NoDrop},Skip,Keep${If[${Corpse.Item[${invSlot}].Stackable},|${Corpse.Item[${invSlot}].StackSize},]}]}]}
			/call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${Corpse.Item[${invSlot}].Container},Container,${If[${Corpse.Item[${invSlot}].NoDrop},Skip,Keep${If[${Corpse.Item[${invSlot}].Stackable},|${Corpse.Item[${invSlot}].StackSize},]}]}]}
			/echo /docommand ${ChatToggle} Added: [${Corpse.Item[${invSlot}].Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
			/docommand ${ChatToggle} Added: [${Corpse.Item[${invSlot}].Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
		}
	| If the item is in a bag
	} else {
		| Reset itemName if it contains a ':'.  ':'s cause errors when reading from the ini, because they act as a delimiter, just like '='s
		/varset itemName ${Me.Inventory[${invSlot}].Item[${itemSlot}].Name}
		/if (${itemName.Find[:]}) /varset itemName ${itemName.Replace[:,;]}
    /if (${itemName.Find[,]}) {
      /call RemoveComma "${itemName}"
      /varset itemName ${Macro.Return}
    }
		| Set item value
		/varset itemValue ${Me.Inventory[${invSlot}].Item[${itemSlot}].Value}
		/varset itemValue ${If[${Bool[${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}]},${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}p,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}g,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}s,]}${If[${Bool[${itemValue.Right[1]}]},${itemValue.Right[1]}c,]}
		| Set ini variables like stack size, (C), (ND) etc.
		/varset iniEntryVariables ${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Stackable},(${Me.Inventory[${invSlot}].Item[${itemSlot}].StackSize}),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].NoDrop},(ND),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Lore},(L),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Container},(C),]}
		| Check for a Loot_Ini entry
		/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length}) {
			
      /if (!(${LootOnlyStackableEnabled} &&  !${Me.Inventory[${invSlot}].Item[${itemSlot}].Stackable})) {
        /call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Container},Container,${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].NoDrop},Skip,Keep${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Stackable},|${Me.Inventory[${invSlot}].Item[${itemSlot}].StackSize},]}]}]}
        /docommand ${ChatToggle} Added: [${Me.Inventory[${invSlot}].Item[${itemSlot}].Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
      }
 			| Add scripts to import old loot settings from ninjaloot.ini, and old e3 loot inis
		}
	}
	/varset lootSetting ${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]}

	/if (${Debug} || ${Debug_Loot}) /echo [get_lootSetting] ${itemName},${lootSetting},invSlot:${invSlot},${itemSlot}
/RETURN

|--------------------------------------------------------------------|
|- Loots items mobs you have killed, to destroy or autoinventory.	  -|
|--------------------------------------------------------------------|
|- @param slotNum: The slot number you are looting from.			      -|
|- @param handle: What to do with the item once you loot it.		    -|
|--------------------------------------------------------------------|
SUB loot_Handle(int slotNum, handle)
  |/if (${Debug} || ${Debug_Loot}) /echo [loot_Handle] slot ${slotNum} ${handle}
	/declare lootTimer timer local 3s
	:retry_Loot
	| Try to loot the specified item from the corpse.
	/itemnotify loot${slotNum} leftmouseup

	/delay 1s ${Cursor.ID} || ${Window[ConfirmationDialogBox].Open} || ${Window[QuantityWnd].Open}
	| If the item has been looted, decide what to do with it.
	/if (${Cursor.ID}) {
		| Destroy the item.
		/if (${handle.Equal[destroy]}) {
			:destroy_Item
			/destroy
			| If the destroy attempt was unsuccessful, try again.	
			/if (${Cursor.ID} && ${lootTimer}) /goto :destroy_Item
		| Else, keep the item.
		} else {
			/call ClearCursor
			/echo Succesfully looted ${slotNum}
		}
	| Else, if loot attempt was unsuccessful, try again.		
	} else {
		| Click yes on confirmation box.
		/if (${Window[ConfirmationDialogBox].Open}) {
			/notify ConfirmationDialogBox Yes_Button leftmouseup
		} else /if (${Window[QuantityWnd].Open}) {
			/notify QuantityWnd QTYW_Accept_Button leftmouseup
		}
		/if (${lootTimer}) /goto :retry_Loot
	}
|/if (${Debug} || ${Debug_Loot}) /echo <== loot_Handle -|
/RETURN

|--------------------------------------------------------------------|
|- Check Loot Setting
#EVENT checkLootSetting "<#*#> Check Loot Setting for #1#"
#EVENT checkLootSetting "Check Loot Setting for #1#"
SUB EVENT_checkLootSetting(line, itemName)
/if (${Debug} || ${Debug_Loot}) /echo |- EVENT_checkLootSetting ==>
	/declare itemToCheck string local ${itemName}
	/if (${itemToCheck.Find[:]}) /varset itemToCheck ${itemToCheck.Replace[:,;]}
  /if (${itemToCheck.Find[,]}) {
    /call RemoveComma "${itemToCheck}"
    /varset itemToCheck ${Macro.Return}
  }
	/declare i int local 1
	
	:check_Loot_Ini
	/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
		/varcalc i ${i} + 1
		/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
			/goto :check_Loot_Ini
		} else {
			/echo No entry found.
		}
	} else {
		/echo >>> [${itemName}=${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}] <<<
	}
/if (${Debug} || ${Debug_Loot}) /echo <== EVENT_checkLootSetting -|
/RETURN

|--------------------------------------------------------------------|
|- Update Loot Setting
#EVENT updateLootSetting "<#1#> Set Loot Setting for #2# /#3#"
#EVENT updateLootSetting "Set Loot Setting for #2# /#3#"
SUB EVENT_updateLootSetting(line, ChatSender, itemName, iniSetting)
/if (${Debug} || ${Debug_Loot}) /echo |- EVENT_updateLootSetting ==>
	/if (${ChatSender.Equal[${Me.CleanName}]} || !${Defined[ChatSender]}) {
		/declare itemToCheck string local ${itemName}
		/if (${itemToCheck.Find[:]}) /varset itemToCheck ${itemToCheck.Replace[:,;]}
    /if (${itemToCheck.Find[,]}) {
      /call RemoveComma "${itemToCheck}"
      /varset itemToCheck ${Macro.Return}
    }
		/declare i int local 1
		
		:check_Loot_Ini
		/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
			/varcalc i ${i} + 1
			/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
				/goto :check_Loot_Ini
			} else {
				/echo No entry found.
			}
		} else {
			/call WriteToIni "${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}" "${iniSetting}" 1
			/echo >>> Loot_Ini entry for [${itemName}] has been changed to [${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}]. <<<
		}
	}
/if (${Debug} || ${Debug_Loot}) /echo <== EVENT_updateLootSetting -|
/RETURN

|--------------------------------------------------------------------|
SUB loot_Setup
/call iniToVarV "${advSettings_Ini},Debug,Debug Loot (On/Off)" Debug_Loot bool outer
/if (${Debug} || ${Debug_Loot}) /echo |- _Setup ==>
	/if (!${Ini[${MacroData_Ini},File Paths,Loot Settings File].Length}) /call WriteToIni "${MacroData_Ini},File Paths,Loot Settings File" "e3 Macro Inis\Loot Settings.ini" 1
	/if (!${Ini[${MacroData_Ini},File Paths,Loot Settings File].Length}) {
		/echo ERROR: Could not find designated file path for [Loot Settings File], please review [e3Data.ini] and restart.
		/endmacro
		/beep
	} else {
		/declare Loot_Ini string outer ${Ini[${MacroData_Ini},File Paths,Loot Settings File]}
	}

  /declare fullInventory_Alert bool outer FALSE
  /declare canLootCorpse bool outer FALSE
  /declare cantLootCorpseCount int outer FALSE
  /declare lootSetting string outer
  /declare LootNumOfFreeSlotsOpen int outer 0

	/if (!${Ini[${Loot_Ini}].Length}) /call Build_Alphabetized_Ini "${Loot_Ini}"
	/call iniToVarV "${genSettings_Ini},Loot,Loot Link Channel" linkChannel string outer
	/call iniToVarV "${genSettings_Ini},Loot,Corpse Seek Radius" seek_Radius int outer
  /call iniToVarV "${genSettings_Ini},Loot,Loot in Combat" combatLooting bool outer
  

	| Import character settings
	/if (${Bool[${Ini[${Character_Ini},Misc,Auto-Loot (On/Off)]}]}) /call iniToVarV "${Character_Ini},Misc,Auto-Loot (On/Off)" Auto_Loot bool outer
  /if (${Ini[${genSettings_Ini},Loot,NumOfFreeSlotsOpen(1+)].Length}) /call iniToVarV "${genSettings_Ini},Loot,NumOfFreeSlotsOpen(1+)" LootNumOfFreeSlotsOpen int outer
  /if (${Ini[${genSettings_Ini},Loot,Loot Only Stackable: Enable (On/Off)].Length}) /call iniToVarV "${genSettings_Ini},Loot,Loot Only Stackable: Enable (On/Off)" LootOnlyStackableEnabled bool outer
  /if (${Ini[${genSettings_Ini},Loot,Loot Only Stackable: With Value Greater Than Or Equal in Copper].Length}) /call iniToVarV "${genSettings_Ini},Loot,Loot Only Stackable: With Value Greater Than Or Equal in Copper" LootOnlyStackableValueGreater int outer
  /if (${Ini[${genSettings_Ini},Loot,Loot Only Stackable: Loot common tradeskill items ie:pelts ores silks etc (On/Off)].Length}) /call iniToVarV "${genSettings_Ini},Loot,Loot Only Stackable: Loot common tradeskill items ie:pelts ores silks etc (On/Off)" LootOnlyStackableTradeSkillsCommon bool outer
  /if (${Ini[${genSettings_Ini},Loot,Loot Only Stackable: Loot all Tradeskill items (On/Off)].Length})  /call iniToVarV "${genSettings_Ini},Loot,Loot Only Stackable: Loot all Tradeskill items (On/Off)" LootOnlyStackableTradeSkillsALL bool outer
  /call IniToArrayV "${genSettings_Ini},Loot,Loot Only Stackable: Always Loot Item#"	stackableItemArray
  |**/if (${Defined[stackableItemArray]}) {
     /declare i int local 
    /for i 1 to ${stackableItemArray.Size}
 /if (${stackableItemArray.Size}==0) /break
      /echo stackable item: ${stackableItemArray[${i}]}
    /next i
  }**|


/if (${Debug} || ${Debug_Loot}) /echo <== _Setup -|
/RETURN

|--------------------------------------------------------------------|
SUB loot_MacroSettings
/if (${Debug}) /echo |- _MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},Debug,Debug Loot (On/Off)" Off
	/call WriteToIni "${genSettings_Ini},Loot,Loot Link Channel" say
	/call WriteToIni "${genSettings_Ini},Loot,Corpse Seek Radius" 125
  /call WriteToIni "${genSettings_Ini},Loot,Loot in Combat" TRUE
  /call WriteToIni "${genSettings_Ini},Loot,NumOfFreeSlotsOpen(1+)" 0
  
/if (${Debug}) /echo <== _MacroSettings -|
/RETURN

|--------------------------------------------------------------------|
SUB loot_CharacterSettings
	/call WriteToIni "${Character_Ini},Misc,Auto-Loot (On/Off)" Off
/RETURN

|--------------------------------------------------------------------|
Sub loot_Aliases
/return

|--------------------------------------------------------------------|
Sub loot_Background_Events
  /doevents lootRelay
  /doevents checkLootSetting
  /doevents updateLootSetting
  /doevents LootUp
/return
|-------------------------------------------------------
|Manually trigger a looting cycle
|
#event LootUp "<#1#> LootUp #2#"
SUB EVENT_LootUp(line, ChatSender, LootSetting)
    /if (${Debug} || ${Debug_Loot}) /echo |- EVENT_LootUp ==>
    /if (!${checkEventArgs[${ChatSender},${line},U,${LootSetting}]}) /return
    /declare AutoLootTrack bool local
    /varset AutoLootTrack ${Auto_Loot}
    /bc Manual Loot Cycle Triggered
    /varset Auto_Loot TRUE
    /call loot_Area
    /varset Auto_Loot ${AutoLootTrack}
    /if (${Debug} || ${Debug_Loot}) /echo <== EVENT_LootUp -|
/RETURN