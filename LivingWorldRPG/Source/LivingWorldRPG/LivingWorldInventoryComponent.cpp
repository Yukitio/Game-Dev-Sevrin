#include "LivingWorldInventoryComponent.h"

int32 ULivingWorldInventoryComponent::AddItem(ULivingWorldItem* Item, int32 Quantity) {
	if (!Item || Quantity <= 0) {
		return 0;
	}

	int32 Remaining = Quantity;

	if (Item->bStackable) {
		for (FLivingWorldInventorySlot& Slot : Slots) {
			if (Remaining <= 0) break;
			if (Slot.Item != Item || Slot.Quantity >= Item->MaxStackSize) continue;
			const int32 SpaceInSlot = Item->MaxStackSize - Slot.Quantity;
			const int32 ToAdd = FMath::Min(SpaceInSlot, Remaining);
			Slot.Quantity += ToAdd;
			Remaining -= ToAdd;
		}
		while (Remaining > 0) {
			FLivingWorldInventorySlot NewSlot;
			NewSlot.Item = Item;
			NewSlot.Quantity = FMath::Min(Remaining, Item->MaxStackSize);
			Slots.Add(NewSlot);
			Remaining -= NewSlot.Quantity;
		}
	} else {
		for (int32 i = 0; i < Quantity; ++i) {
			FLivingWorldInventorySlot NewSlot;
			NewSlot.Item = Item;
			NewSlot.Quantity = 1;
			Slots.Add(NewSlot);
		}
		Remaining = 0;
	}

	const int32 Added = Quantity - Remaining;
	if (Added > 0) {
		OnInventoryChanged.Broadcast();
	}
	return Added;
}

bool ULivingWorldInventoryComponent::RemoveItem(ULivingWorldItem* Item, int32 Quantity) {
	if (!Item || Quantity <= 0 || GetQuantity(Item) < Quantity) {
		return false;
	}

	int32 Remaining = Quantity;
	for (int32 i = Slots.Num() - 1; i >= 0 && Remaining > 0; --i) {
		FLivingWorldInventorySlot& Slot = Slots[i];
		if (Slot.Item != Item) continue;
		const int32 ToRemove = FMath::Min(Slot.Quantity, Remaining);
		Slot.Quantity -= ToRemove;
		Remaining -= ToRemove;
		if (Slot.Quantity <= 0) {
			Slots.RemoveAt(i);
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

int32 ULivingWorldInventoryComponent::GetQuantity(const ULivingWorldItem* Item) const {
	int32 Total = 0;
	for (const FLivingWorldInventorySlot& Slot : Slots) {
		if (Slot.Item == Item) {
			Total += Slot.Quantity;
		}
	}
	return Total;
}

bool ULivingWorldInventoryComponent::HasItem(const ULivingWorldItem* Item, int32 Quantity) const {
	return GetQuantity(Item) >= Quantity;
}
