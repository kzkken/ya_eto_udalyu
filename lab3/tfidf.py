import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer


class TfidfScorer:
    def __init__(self, stop_words, top_k=3):
        self.vectorizer = TfidfVectorizer(stop_words=list(stop_words))
        self.top_k = top_k 

    def score_sentences(self, clean_sentences):
        if len(clean_sentences) == 0:
            return []

        tfidf_matrix = self.vectorizer.fit_transform(clean_sentences)
        matrix = tfidf_matrix.toarray()

        scores = []
        for row in matrix:
            if len(row) == 0 or np.sum(row) == 0:
                scores.append(0.0)
            else:
                top_values = np.sort(row)[-self.top_k:]
                scores.append(np.mean(top_values))
        
        return np.array(scores)