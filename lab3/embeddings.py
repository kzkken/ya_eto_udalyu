import numpy as np
from sentence_transformers import SentenceTransformer
from sklearn.metrics.pairwise import cosine_similarity


class EmbeddingScorer:
    def __init__(self, model_name="cointegrated/rubert-tiny2"):
        self.model = SentenceTransformer(model_name)

    def encode_sentences(self, sentences):
        return self.model.encode(sentences)

    def score_sentences(self, sentences):

        if len(sentences) == 0:
            return [], None

        embeddings = self.encode_sentences(sentences)

        centroid = np.mean(embeddings, axis=0)

        scores = cosine_similarity(embeddings, centroid.reshape(1, -1)).flatten()

        return scores, embeddings