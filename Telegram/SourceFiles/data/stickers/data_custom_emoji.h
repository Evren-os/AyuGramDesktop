/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "data/stickers/data_stickers_set.h"
#include "ui/text/custom_emoji_instance.h"
#include "base/timer.h"
#include "base/weak_ptr.h"

struct StickerSetIdentifier;

namespace Main {
class Session;
} // namespace Main

namespace Data {

class Session;
class CustomEmojiLoader;

struct CustomEmojiId {
	uint64 selfId = 0;
	uint64 id = 0;
};

class CustomEmojiManager final : public base::has_weak_ptr {
public:
	enum class SizeTag {
		Normal,
		Large,
	};

	CustomEmojiManager(not_null<Session*> owner);
	~CustomEmojiManager();

	[[nodiscard]] std::unique_ptr<Ui::Text::CustomEmoji> create(
		QStringView data,
		Fn<void()> update,
		SizeTag tag = SizeTag::Normal);
	[[nodiscard]] std::unique_ptr<Ui::Text::CustomEmoji> create(
		DocumentId documentId,
		Fn<void()> update,
		SizeTag tag = SizeTag::Normal);
	[[nodiscard]] std::unique_ptr<Ui::Text::CustomEmoji> create(
		not_null<DocumentData*> document,
		Fn<void()> update,
		SizeTag tag = SizeTag::Normal);

	[[nodiscard]] std::unique_ptr<Ui::CustomEmoji::Loader> createLoader(
		not_null<DocumentData*> document,
		SizeTag tag);
	[[nodiscard]] std::unique_ptr<Ui::CustomEmoji::Loader> createLoader(
		DocumentId documentId,
		SizeTag tag);

	[[nodiscard]] QString lookupSetName(uint64 setId);

	[[nodiscard]] Main::Session &session() const;
	[[nodiscard]] Session &owner() const;

private:
	struct RepaintBunch {
		crl::time when = 0;
		std::vector<base::weak_ptr<Ui::CustomEmoji::Instance>> instances;
	};

	void request();
	void requestFinished();
	void repaintLater(
		not_null<Ui::CustomEmoji::Instance*> instance,
		Ui::CustomEmoji::RepaintRequest request);
	void scheduleRepaintTimer();
	void invokeRepaints();
	void requestSetFor(not_null<DocumentData*> document);

	[[nodiscard]] Ui::CustomEmoji::Preview prepareNonExactPreview(
		DocumentId documentId,
		SizeTag tag) const;
	template <typename LoaderFactory>
	[[nodiscard]] std::unique_ptr<Ui::Text::CustomEmoji> create(
		DocumentId documentId,
		Fn<void()> update,
		SizeTag tag,
		LoaderFactory factory);
	[[nodiscard]] static int SizeIndex(SizeTag tag);

	const not_null<Session*> _owner;

	base::flat_map<
		uint64,
		std::unique_ptr<Ui::CustomEmoji::Instance>> _instances[2];
	base::flat_map<
		uint64,
		std::vector<base::weak_ptr<CustomEmojiLoader>>> _loaders[2];
	base::flat_set<uint64> _pendingForRequest;
	mtpRequestId _requestId = 0;

	base::flat_map<crl::time, RepaintBunch> _repaints;
	crl::time _repaintNext = 0;
	base::Timer _repaintTimer;
	bool _repaintTimerScheduled = false;
	bool _requestSetsScheduled = false;

};

[[nodiscard]] QString SerializeCustomEmojiId(const CustomEmojiId &id);
[[nodiscard]] QString SerializeCustomEmojiId(
	not_null<DocumentData*> document);
[[nodiscard]] CustomEmojiId ParseCustomEmojiData(QStringView data);

void InsertCustomEmoji(
	not_null<Ui::InputField*> field,
	not_null<DocumentData*> document);

} // namespace Data