import nltk
from nltk.tokenize import sent_tokenize, word_tokenize
from nltk.corpus import stopwords
import pymorphy3
import string

def download_nltk_data():
    resources = ['punkt', 'punkt_tab', 'stopwords']
    for resource in resources:
        try:
            nltk.data.find(f'tokenizers/{resource}')
        except LookupError:
            try:
                nltk.data.find(f'corpora/{resource}')
            except LookupError:
                nltk.download(resource, quiet=True)


download_nltk_data()


class TextPreprocessor:
    def __init__(self):
        self.stop_words = set(stopwords.words('russian'))
        self.morph = pymorphy3.MorphAnalyzer()

    def split_sentences(self, text: str):
        sentences = sent_tokenize(text, language='russian')
        return sentences

    def clean_sentence(self, sentence: str):
        tokens = word_tokenize(sentence.lower(), language='russian')
        
        filtered = []
        for token in tokens:
            if token not in string.punctuation and token not in self.stop_words:
                lemma = self.morph.parse(token)[0].normal_form
                filtered.append(lemma)
        
        return " ".join(filtered)

    def preprocess(self, text: str):
        sentences = self.split_sentences(text)
        clean_sentences = [self.clean_sentence(s) for s in sentences]

        filtered_pairs = [(orig, clean) for orig, clean in zip(sentences, clean_sentences) if clean.strip()]
        
        if not filtered_pairs:
            return sentences, clean_sentences
        
        sentences_filtered = [pair[0] for pair in filtered_pairs]
        clean_sentences_filtered = [pair[1] for pair in filtered_pairs]

        return sentences_filtered, clean_sentences_filtered