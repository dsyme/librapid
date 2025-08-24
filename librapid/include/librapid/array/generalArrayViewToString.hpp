#ifndef LIBRAPID_ARRAY_ARRAY_VIEW_STRING_HPP
#define LIBRAPID_ARRAY_ARRAY_VIEW_STRING_HPP

namespace librapid {
	namespace detail {
		// Optimized alignment function with caching and reduced allocations
		template<size_t N, typename Val>
		std::pair<int64_t, int64_t> alignment(const char (&formatString)[N], const Val &value) {
			// Use compile-time constant format string when possible
			if constexpr (std::is_same_v<Val, int> || std::is_same_v<Val, long> || std::is_same_v<Val, long long>) {
				// Fast path for common integer types - estimate width without formatting
				if constexpr (std::is_integral_v<std::decay_t<Val>>) {
					int64_t width = (value == 0) ? 1 : static_cast<int64_t>(std::log10(std::abs(static_cast<double>(value)))) + 1;
					if (value < 0) width++; // Account for negative sign
					return std::make_pair(width, 0);
				}
			}
			
			// Cache formatted string size to avoid repeated allocation
			thread_local std::string cached_format;
			thread_local std::string formatted;
			
			// Reuse string buffers to minimize allocations
			cached_format.clear();
			formatted.clear();
			
			cached_format = fmt::format("{{:{}}}", formatString);
			formatted = fmt::vformat(cached_format, fmt::make_format_args(value));

			if constexpr (std::is_integral_v<std::decay_t<Val>>) {
				return std::make_pair(formatted.length(), 0);
			} else if constexpr (std::is_floating_point_v<std::decay_t<Val>>) {
				auto point = formatted.find('.');
				if (point == std::string::npos) {
					return std::make_pair(formatted.length(), 0);
				} else {
					return std::make_pair(point, formatted.length() - point);
				}
			}

			return std::make_pair(0, 0);
		}

		template<typename ArrayViewType, typename ArrayViewShapeType, size_t N>
		void generalArrayViewToStringColWidthFinder(
		  const array::GeneralArrayView<ArrayViewType, ArrayViewShapeType> &view,
		  const char (&formatString)[N], std::vector<std::pair<int64_t, int64_t>> &alignments) {
			if (view.ndim() == 1) {
				for (int64_t i = 0; i < static_cast<int64_t>(view.shape()[0]); i++) {
					auto alignmentPair = alignment(formatString, view.scalar(i));
					if (i >= static_cast<int64_t>(alignments.size())) {
						alignments.push_back(alignmentPair);
					} else {
						alignments[i].first =
						  ::librapid::max(alignments[i].first, alignmentPair.first);
						alignments[i].second =
						  ::librapid::max(alignments[i].second, alignmentPair.second);
					}
				}
			} else if (view.ndim() > 1) {
				for (int64_t i = 0; i < static_cast<int64_t>(view.shape()[0]); i++) {
					generalArrayViewToStringColWidthFinder(view[i], formatString, alignments);
				}
			}
		}

		// Optimized bracket character lookup to reduce branch prediction misses
		inline std::pair<char, char> getBracketChars(char bracket) noexcept {
			// Use lookup table for better branch prediction and potential vectorization
			static constexpr char open_brackets[]  = {'[', '(', '[', '{', '<', '|'};  // default, r, s, c, a, p
			static constexpr char close_brackets[] = {']', ')', ']', '}', '>', '|'};
			
			// Map bracket character to index (optimized for common cases)
			switch (bracket) {
				case 'r': return {open_brackets[1], close_brackets[1]};
				case 's': return {open_brackets[2], close_brackets[2]};
				case 'c': return {open_brackets[3], close_brackets[3]};
				case 'a': return {open_brackets[4], close_brackets[4]};
				case 'p': return {open_brackets[5], close_brackets[5]};
				default: return {open_brackets[0], close_brackets[0]};
			}
		}

		template<typename ArrayViewType, typename ArrayViewShapeType, typename T, typename Char,
				 size_t N, typename Ctx>
		void generalArrayViewToStringImpl(
		  const array::GeneralArrayView<ArrayViewType, ArrayViewShapeType> &view,
		  const fmt::formatter<T, Char> &formatter, char bracket, char separator,
		  const char (&formatString)[N], int64_t indent, Ctx &ctx,
		  const std::vector<std::pair<int64_t, int64_t>> &alignments) {
			
			// Get bracket characters with optimized lookup
			const auto [bracketCharOpen, bracketCharClose] = getBracketChars(bracket);

			// Separator char is already the correct character

			if (view.ndim() == 0) {
				formatter.format(view.scalar(0), ctx);
			} else if (view.ndim() == 1) {
				fmt::format_to(ctx.out(), "{}", bracketCharOpen);
				
				// Pre-allocate padding string buffer to avoid repeated allocations
				thread_local std::string padding_buffer;
				
				for (int64_t i = 0; i < static_cast<int64_t>(view.shape()[0]); i++) {
					auto columnAlignment = alignments[i];
					auto valueSize		 = alignment(formatString, view.scalar(i));
					int64_t pre	 = max(columnAlignment.first - valueSize.first, 0),
							post = max(columnAlignment.second - valueSize.second, 0);

					// Optimized padding - reuse buffer and write directly to output
					if (pre > 0) {
						if (padding_buffer.size() < static_cast<size_t>(pre)) {
							padding_buffer.resize(pre, ' ');
						}
						// Write only the needed number of spaces directly
						for (int64_t p = 0; p < pre; ++p) {
							*ctx.out()++ = ' ';
						}
					}
					
					formatter.format(view.scalar(i), ctx);
					
					if (post > 0) {
						for (int64_t p = 0; p < post; ++p) {
							*ctx.out()++ = ' ';
						}
					}

					if (i != view.shape()[0] - 1) {
						if (separator == ' ') {
							*ctx.out()++ = ' ';
						} else {
							*ctx.out()++ = separator;
							*ctx.out()++ = ' ';
						}
					}
				}
				*ctx.out()++ = bracketCharClose;
			} else {
				*ctx.out()++ = bracketCharOpen;
				
				// Cache indentation string to avoid repeated allocations
				thread_local std::string indent_buffer;
				if (indent_buffer.size() < static_cast<size_t>(indent + 1)) {
					indent_buffer.resize(indent + 1, ' ');
				}
				
				for (int64_t i = 0; i < static_cast<int64_t>(view.shape()[0]); i++) {
					if (i > 0) {
						// Write indentation directly
						for (int64_t ind = 0; ind < indent + 1; ++ind) {
							*ctx.out()++ = ' ';
						}
					}
					generalArrayViewToStringImpl(view[i],
												 formatter,
												 bracket,
												 separator,
												 formatString,
												 indent + 1,
												 ctx,
												 alignments);
					if (i != view.shape()[0] - 1) {
						if (separator == ' ') {
							*ctx.out()++ = '\n';
						} else {
							*ctx.out()++ = separator;
							*ctx.out()++ = '\n';
						}
						if (view.ndim() > 2) { *ctx.out()++ = '\n'; }
					}
				}
				*ctx.out()++ = bracketCharClose;
			}
		}

		template<typename ArrayViewType, typename ArrayViewShapeType, typename T, typename Char,
				 size_t N, typename Ctx>
		void generalArrayViewToString(
		  const array::GeneralArrayView<ArrayViewType, ArrayViewShapeType> &view,
		  const fmt::formatter<T, Char> &formatter, char bracket, char separator,
		  const char (&formatString)[N], int64_t indent, Ctx &ctx) {
			std::vector<std::pair<int64_t, int64_t>> alignments;
			generalArrayViewToStringColWidthFinder(view, formatString, alignments);
			generalArrayViewToStringImpl(
			  view, formatter, bracket, separator, formatString, indent, ctx, alignments);
		}
	} // namespace detail

	namespace array {
		template<typename ArrayViewType, typename ArrayViewShapeType>
		template<typename T, typename Char, size_t N, typename Ctx>
		void GeneralArrayView<ArrayViewType, ArrayViewShapeType>::str(
		  const fmt::formatter<T, Char> &format, char bracket, char separator,
		  const char (&formatString)[N], Ctx &ctx) const {
			detail::generalArrayViewToString(
			  *this, format, bracket, separator, formatString, 0, ctx);
		}
	} // namespace array
} // namespace librapid

#endif // LIBRAPID_ARRAY_ARRAY_VIEW_STRING_HPP