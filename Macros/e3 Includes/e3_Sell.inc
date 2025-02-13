|------------------------------------------------------------------------|
|- e3_Macro_Sell.inc v1.5 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Companion file for e3_Macro_Loot.inc.								-|
|- Sells items flagged with [/Sell] in your Loot_Ini.					-|
|------------------------------------------------------------------------|



|----------------------------------------------------|
|- syncInventory									-|
|----------------------------------------------------|
|- Adds items from you inventory to your Loot_Ini.	-|
|----------------------------------------------------|
#EVENT syncInventory "<#*#> Inventory Items"
#EVENT syncInventory "Inventory Items"
#EVENT syncInventory "<#*#> Update Loot.ini"
#EVENT syncInventory "Update Loot.ini"
#EVENT syncInventory "<#*#> Sync Inventory"
#EVENT syncInventory "Sync Inventory"
#EVENT syncInventory "<#*#> Sync Loot.ini"
#EVENT syncInventory "Sync Loot.ini"
SUB EVENT_syncInventory
/if (${Debug} || ${Debug_Sell}) /echo |- event_inventoryItems ==>
	
	/echo Syncing inventory with ${Loot_Ini}...

	/declare i int local
	/declare x int local

	| Check inventory slots 1 - ${numInventorySlots} for items, and containers.
	/for i 1 to ${numInventorySlots}
 /if (${numInventorySlots}==0) /break
	
		| If there is an item in inventory slot ${i}
		/if (${Bool[${InvSlot[pack${i}].Item}]}) {
			/call get_lootSetting "pack${i}" 0
		
			| If the item is a container..
			/if (${InvSlot[pack${i}].Item.Container}) {
			
				/for x 1 to ${InvSlot[pack${i}].Item.Container}
 /if (${InvSlot[pack${i}].Item.Container}==0) /break
			
					/if (${Bool[${Me.Inventory[pack${i}].Item[${x}]}]}) /call get_lootSetting "pack${i}" "${x}"
					
				/next x
			}
		}
	
	/next i
	
/if (${Debug} || ${Debug_Sell}) /echo <== event_inventoryItems -|
/RETURN



|------------------------------------------------------------------------------------------------------------------------------------|
|- EVENT_sellItems																													-|
|------------------------------------------------------------------------------------------------------------------------------------|
|- Commands bots, including yourself, to open trade with the nearest merchant, and sell items flagged as [/Sell] in your Loot_Ini.	-|
|------------------------------------------------------------------------------------------------------------------------------------|
#EVENT sellItems "Sell Items"
#EVENT sellItems "<#*#> Sell Items"
#EVENT sellItems "AutoSell"
#EVENT sellItems "<#*#> AutoSell"
#EVENT sellItems "Auto Sell"
#EVENT sellItems "<#*#> Auto Sell"
SUB EVENT_sellItems
/if (${Debug} || ${Debug_Sell}) /echo |- EVENT_sellItems ==>
	
	| Record starting location.
	/declare startingLoc string local ${Me.Loc.Replace[ ,]}
		
	| Sync inventory with Loot_Ini.
	| /call EVENT_syncInventory
		
	| Open trade with the nearest merchant.
	/call openMerchant
		
	/if (${Window[MerchantWnd].Open}) {
	
		| Open bags.
		/keypress OPEN_INV_BAGS
	
		| Sell Items.
		/call sellItems
		
		| Move back to starting location.	
		/squelch /moveto loc ${startingLoc.Arg[1,,]} ${startingLoc.Arg[2,,]}
		
		| Close merchant.
		/if (${Window[MerchantWnd].Open}) /call closeMerchant
	
		| Destroy bad items.
		/if (${DestroyableItems.Size}) /call destroyItems
	
		| Close bags.
		/keypress CLOSE_INV_BAGS
		
		/echo I have finished selling.
	}
	
/if (${Debug} || ${Debug_Sell}) /echo <== EVENT_sellItems -|
/RETURN



|----------------------------------------|
|- Opens all containers in inventory.	-|
|----------------------------------------|
SUB openBags
/if (${Debug} || ${Debug_Sell}) /echo |- openBags ==>

	| Open all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to ${numInventorySlots}
 /if (${numInventorySlots}==0) /break
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
			:openBag
			/if (${Debug} || ${Debug_Sell}) /echo |- openBags -| :openBag
		
			| Open container
			/if (!${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 ${Window[pack${i}].Open} || ${Window[TradeSkillWnd].Open}
				/if (!${Window[pack${i}].Open} && !${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openBag
				
				| Open tradeskill container.
				/if (${Window[TradeSkillWnd].Open}) {
					:openExperimentWnd
					/notify TradeSkillWnd ExperimentButton leftmouseup
					/delay 5 !${Window[TradeSkillWnd].Open}
					/if (${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openExperimentWnd
				}
			}
		}
	
	/next i
	
/if (${Debug} || ${Debug_Sell}) /echo <== openBags -|
/RETURN



|----------------------------------------|
|- Closes all containers in inventory.	-|
|----------------------------------------|
SUB closeBags
/if (${Debug} || ${Debug_Sell}) /echo |- closeBags ==>

	| Close all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to ${numInventorySlots}
 /if (${numInventorySlots}==0) /break
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
		:closeBag
		/if (${Debug} || ${Debug_Sell}) /echo |- closeBags -| :closeBag
		
			| Open container
			/if (${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 !${Window[pack${i}].Open}
				/if (${Window[pack${i}].Open} && ${miscTimer}) /goto :closeBag
			}
		}
	
	/next i
	
/if (${Debug} || ${Debug_Sell}) /echo <== closeBags -|
/RETURN



|--------------------------------------------|
|- Opens trade with the nearest merchant.	-|
|--------------------------------------------|
SUB openMerchant(int providedID)
/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant ==>

	/declare miscTimer timer local
	/declare merchantTotal int local -1
	/declare merchantID int local ${If[${providedID},${providedID},${Spawn[Merchant].ID}]}

	/if (!${Spawn[Merchant].ID} || !${Spawn[Merchant].LineOfSight}) {
		/echo There are no merchants nearby!
	} else {
	
		| Move to the merchant.
		/call TrueTarget ${merchantID}
		/call MoveToLoc ${Target.Y} ${Target.X} 80 15
	
		| Open Trade with merchant.
		/if (${Target.Distance} < 20) {
		
			| Set timer
			/varset miscTimer 100
		
			| Right click merchant, and wait for window to open.
			:openMerchant
			/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant -| :openMerchant
			
			/if (!${Window[MerchantWnd].Open}) /click right target
			/delay 5 ${Window[MerchantWnd].Open}
			/if (!${Window[MerchantWnd].Open}) {
				/if (${miscTimer}) {
					/goto :openMerchant
				} else {
					/echo Failed to open trade with [${Target.CleanName}].
				}
			}
			
			| Wait for merchant's item list to populate.
			:populateMerchant
			/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant -| :populateMerchant
			/if (${merchantTotal}!=${Window[MerchantWnd].Child[ItemList].Items}) {
				/varset merchantTotal ${Window[MerchantWnd].Child[ItemList].Items}
				/delay 2
				/if (${miscTimer}) /goto :populateMerchant
			}
		}
	}

/if (${Debug} || ${Debug_Sell}) /echo <== openMerchant -|
/RETURN



|--------------------|
|- closeMerchant	-|
|--------------------|
SUB closeMerchant
/if (${Debug} || ${Debug_Sell}) /echo |- closeMerchant ==>

	/declare miscTimer timer local 50
	
	:closeMerchant
	/if (${Debug} || ${Debug_Sell}) /echo |- closeMerchant -| :closeMerchant
	/notify MerchantWnd MW_Done_Button leftmouseup
	/delay 5 ${Window[MerchantWnd].Open}
	/if (${Window[MerchantWnd].Open} && ${miscTimer}) /goto :closeMerchant
	
	
/if (${Debug} || ${Debug_Sell}) /echo <== closeMerchant -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Sells items in your inventory that have been flagged as [/Sell] in your Loot_Ini.	-|
|----------------------------------------------------------------------------------------|
SUB sellItems
/if (${Debug} || ${Debug_Sell}) /echo |- sellItems ==>
	/declare retryTimer timer local
	/declare i int local
	/declare e int local
	
	| Scan inventory for items to sell
	/for i 1 to ${numInventorySlots}
 /if (${numInventorySlots}==0) /break
	
		/if (${Bool[${Me.Inventory[pack${i}]}]}) {
		
			| If the item in pack slot 'i' IS a container
			/if (${Me.Inventory[pack${i}].Container}) {
			
				| Look at each bag slot for items to sell
				/for e 1 to ${Me.Inventory[pack${i}].Container}
 					/if (${Me.Inventory[pack${i}].Container}==0) /break
				
					/if (${Bool[${Me.Inventory[pack${i}].Item[${e}]}]}) {
					
						| Get the loot setting for the item in pack slot 'i' slot 'e'
						/call get_lootSetting "pack${i}" "${e}"
					
						| Destroy the item
						/if (${lootSetting.Find[Destroy]}) {
							/echo Flagging [${Me.Inventory[pack${i}].Item[${e}]}] to be destroyed...
							/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
						
						| Sell the item
						} else /if (${lootSetting.Find[Sell]}) {
					
							| Check that the item has value.
							/if (!${Me.Inventory[pack${i}].Item[${e}].Value}) {
								/if (${destroyUnsold}) {
									/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}].Item[${e}]}].  Flagging it to be destroyed.
									/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
								}
							} else {
					
								| Select the item to sell
								/varset retryTimer 30

								:retrySelect_Pack
								/if (${Debug} || ${Debug_Sell}) /echo |- sellItems -| :retrySelect_Pack
								/itemnotify in pack${i} ${e} leftmouseup
								/delay 3
								| If the item was not selected
								/if (!${Window[MerchantWnd].Child[MW_SelectedItemLabel].Text.Equal[${Me.Inventory[pack${i}].Item[${e}]}]}) {
									
									| Try again
									/delay 3
									/if (!${Window[MerchantWnd].Child[MW_SelectedItemLabel].Text.Equal[${Me.Inventory[pack${i}].Item[${e}]}]}) {
										/echo ERROR: Failed to select [${Me.Inventory[pack${i}].Item[${e}]}], skipping.
									}
									
								} else {
						
									| Check that the buy button is enabled
									/if (!${Window[MerchantWnd].Child[MW_Sell_Button].Enabled}) {
										/if (${destroyUnsold}) {
											/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}].Item[${e}]}].  Flagging it to be destroyed.
											/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
										}
									} else {
						
										| Sell the item
										/varset retryTimer 30
										/echo [${Time}] Selling from pack [${Me.Inventory[pack${i}].Item[${e}]}].
										
										:SellItem_Pack
										/if (${Debug} || ${Debug_Sell}) /echo |- sellItems -| :SellItem_Pack
										/notify MerchantWnd MW_Sell_Button leftmouseup
										/delay 1
										| Close the quantity window
										/if (${Window[QuantityWnd].Open}) {
											/notify QuantityWnd QTYW_Accept_Button leftmouseup
											/delay 3
										} else {
											|leave a total delay of 3 for client-server communication
											/delay 2
										}

										| If the item is still in my inventory
										/if (${Bool[${Me.Inventory[pack${i}].Item[${e}]}]} && ${retryTimer}) {
											/goto :SellItem_Pack
										} else /if (${Bool[${Me.Inventory[pack${i}].Item[${e}]}]}) {
											/echo ERROR: Failed to sell [${Me.Inventory[pack${i}].Item[${e}]}], skipping.
										}
									}
								}
							}
						}
					}
				
				/next e
			
			| If the item in pack slot 'i' is NOT a container
			} else {
			
				| Get the loot setting for the item in pack slot 'i'
				/call get_lootSetting "pack${i}" 0
			
				| Destroy the item
				/if (${lootSetting.Find[Destroy]}) {
					/echo Flagging [${InvSlot[pack${i}].Item.Name}] to be destroyed...
					/call BuildArray DestroyableItems "${InvSlot[pack${i}].Item.Name}"
				
				| Sell the item
				} else /if (${lootSetting.Find[Sell]}) {
				
					| Check that the item has value
					/if (!${Me.Inventory[pack${i}].Item.Value}) {
						/if (${destroyUnsold}) {
							/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}].Item.Name}].  Flagging it to be destroyed.
							/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item.Name}"
						}
					} else {
				
						| Select the item to sell
						/varset retryTimer 30

						:retrySelect
						/if (${Debug} || ${Debug_Sell}) /echo |- sellItems -| :retrySelect
						/itemnotify in pack${i} leftmouseup
						
						/delay 3
						| If the item was not selected
						/if (!${Window[MerchantWnd].Child[MW_SelectedItemLabel].Text.Equal[${Me.Inventory[pack${i}]}]}) {
							
							/delay 3
							/if (!${Window[MerchantWnd].Child[MW_SelectedItemLabel].Text.Equal[${Me.Inventory[pack${i}]}]}) {
								/echo ERROR: Failed to select [${Me.Inventory[pack${i}]}], skipping.
							}
							
						} else {
				
							| Check that the buy button is enabled
							/if (!${Window[MerchantWnd].Child[MW_Sell_Button].Enabled}) {
								/if (${destroyUnsold}) {
									/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}]}].  Flagging it to be destroyed.
									/call BuildArray DestroyableItems "${Me.Inventory[pack${i}]}"
								}
							} else {
				
								| Sell the item
								/varset retryTimer 30
								/echo [${Time}] Selling from root [${Me.Inventory[pack${i}]}].
								
								:SellItem
								/if (${Debug} || ${Debug_Sell}) /echo |- sellItems -| :SellItem
								/notify MerchantWnd MW_Sell_Button leftmouseup
								|give it a moment to show the qty window
								/delay 1
								| Close the quantity window if it opened
								:CheckQtyWindow
								/if (${Window[QuantityWnd].Open}) {
									/notify QuantityWnd QTYW_Accept_Button leftmouseup
									/delay 3
									/if (${Window[QuantityWnd].Open}) { 
										/goto :CheckQtyWindow
									}
								} else {
									|leave a total delay of 3 for client-server communication
									/delay 2
								}
								| If the item is still in my inventory
								/if (${Bool[${Me.Inventory[pack${i}]}]} && ${retryTimer}) {
									/goto :SellItem
								} else /if (${Bool[${Me.Inventory[pack${i}]}]}) {
									/echo ERROR: Failed to sell [${Me.Inventory[pack${i}]}], skipping.
								}
							}
						}
					}
				}
			}
		}
		
	/next i

/if (${Debug} || ${Debug_Sell}) /echo <== sellItems -|
/RETURN



SUB destroyItems
/if (${Debug} || ${Debug_Sell}) /echo |- destroyItem ==>
	
	/declare i int local
	/declare e int local
	/declare y int local
	
	/declare retryTimer timer local
	/declare badItem string local
	
	/for i 1 to ${DestroyableItems.Size}
 /if (${DestroyableItems.Size}==0) /break
	
		/varset badItem ${DestroyableItems[${i}]}

		/for e 1 to ${numInventorySlots}
 /if (${numInventorySlots}==0) /break
		
			/if (!${Me.Inventory[pack${e}].Container}) {
				
				/if (${badItem.Equal[${Me.Inventory[pack${e}]}]}) {
					/varset retryTimer 50
					
					:retry_SlotPickup
					/itemnotify pack${e} leftmouseup
					/delay 5 ${Cursor.ID} || ${Window[QuantityWnd].Open}
					
					| If the quantity window is open.
					/if (${Window[QuantityWnd].Open}) {
						/notify QuantityWnd QTYW_Accept_Button leftmouseup
						/delay 10 !${Window[QuantityWnd].Open}
					}
					
					/delay 5 ${Cursor.ID}
					
					/if (${badItem.Equal[${Cursor}]}) {
						/echo Destroying [${Cursor}].
						/destroy
					} else {
						/if (${retryTimer}) {
							/goto :retry_SlotPickup
						} else {
							/echo ERROR: I failed to destroy [${badItem}], skipping.
						}
					}
				}
			} else {
			
				/for y 1 to ${Me.Inventory[pack${e}].Container}
 /if (${Me.Inventory[pack${e}].Container}==0) /break
				
					/if (${badItem.Equal[${Me.Inventory[pack${e}].Item[${y}]}]}) {
					
						/varset retryTimer 50
					
						:retry_BagPickup
						/itemnotify in pack${e} ${y} leftmouseup
						/delay 5 ${Cursor.ID} || ${Window[QuantityWnd].Open}
						
						| If the quantity window is open.
						/if (${Window[QuantityWnd].Open}) {
							/notify QuantityWnd QTYW_Accept_Button leftmouseup
							/delay 10 !${Window[QuantityWnd].Open}
						}
						
						/delay 5 ${Cursor.ID}
						
						/if (${badItem.Equal[${Cursor}]}) {
							/echo Destroying [${Cursor}].
							/destroy
						} else {
							/if (${retryTimer}) {
								/goto :retry_BagPickup
							} else {
								/echo ERROR: I failed to destroy [${badItem}], skipping.
							}
						}
					}
				/next y
			}
		
		/next e
		
	/next i

/if (${Debug} || ${Debug_Sell}) /echo <== destroyItem -|
/RETURN



SUB sell_Setup
/call iniToVarV "${advSettings_Ini},Debug,Debug Sell (On/Off)" Debug_Sell bool outer
/if (${Debug} || ${Debug_Sell}) /echo |- sell_Setup ==>

	/call iniToVarV "${genSettings_Ini},Sell,Destroy Unsold Items(On/Off)" destroyUnsold bool outer

/if (${Debug} || ${Debug_Sell}) /echo <== sell_Setup -|
/RETURN


 
Sub sell_Background_Events
	/doevents syncInventory
	/doevents sellItems
  /doevents Combine
/return



SUB sell_MacroSettings
/if (${Debug}) /echo |- _MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},Debug,Debug Sell (On/Off)" Off
	/call WriteToIni "${genSettings_Ini},Sell,Destroy Unsold Items(On/Off)" Off
/if (${Debug}) /echo <== _MacroSettings -|
/RETURN



SUB sell_CharacterSettings
/RETURN



Sub sell_Aliases
/if (${Debug}) /echo |- _Aliases ==>
	/squelch /alias /syncInventory /bc Sync Inventory
	/squelch /alias /autosell /echo Auto Sell
/if (${Debug}) /echo <== _Aliases -|
/return