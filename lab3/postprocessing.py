class SummaryBuilder:
    def __init__(self, max_length=300):
        self.max_length = max_length

    def build(self, ranked_sentences):

        if not ranked_sentences:
            return ""

        summary = []
        current_length = 0

        for index, sentence, score in ranked_sentences:
            sentence = sentence.strip()

            if current_length + len(sentence) + 1 > self.max_length:
                continue

            summary.append(sentence)
            current_length += len(sentence) + 1  

        if not summary and ranked_sentences:
            first_sentence = ranked_sentences[0][1].strip()
            return first_sentence[:self.max_length]

        result = " ".join(summary)

        return result[:self.max_length]