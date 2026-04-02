import numpy as np
from sklearn.metrics.pairwise import cosine_similarity


class Ranker:
    def __init__(self, tfidf_weight=0.3, embed_weight=0.5, position_weight=0.2, lambda_mmr=0.7):
        total = tfidf_weight + embed_weight + position_weight
        if not np.isclose(total, 1.0):
            raise ValueError(f"Веса должны суммироваться в 1.0, получено: {total}")
        
        self.tfidf_weight = tfidf_weight
        self.embed_weight = embed_weight
        self.position_weight = position_weight
        self.lambda_mmr = lambda_mmr

    def rank(self, sentences, tfidf_scores, embed_scores):

        if len(sentences) == 0:
            return []

        tfidf_scores = np.array(tfidf_scores)
        embed_scores = np.array(embed_scores)

        n = len(sentences)
        position_scores = np.array([1.0 - (i / n) for i in range(n)])

        def normalize(x):
            if np.max(x) - np.min(x) == 0:
                return x
            return (x - np.min(x)) / (np.max(x) - np.min(x))

        tfidf_norm = normalize(tfidf_scores)
        embed_norm = normalize(embed_scores)
        position_norm = normalize(position_scores)

        final_scores = (
            self.tfidf_weight * tfidf_norm +
            self.embed_weight * embed_norm +
            self.position_weight * position_norm
        )

        indexed_sentences = [(i, sent, score) for i, (sent, score) in enumerate(zip(sentences, final_scores))]
        ranked = sorted(indexed_sentences, key=lambda x: x[2], reverse=True)

        return ranked

    def apply_mmr(self, ranked_sentences, embeddings, top_k=None):
        if len(ranked_sentences) == 0:
            return []
        
        if embeddings is None or len(embeddings) == 0:
            result = ranked_sentences[:top_k] if top_k else ranked_sentences
            return sorted(result, key=lambda x: x[0])
        
        selected = []
        remaining = list(range(len(ranked_sentences)))
        
        first_idx = 0
        selected.append(ranked_sentences[first_idx])
        remaining.remove(first_idx)
        
        while remaining and (top_k is None or len(selected) < top_k):
            mmr_scores = []
            
            for idx in remaining:
                orig_idx, sent, relevance = ranked_sentences[idx]
                
                rel_score = relevance
                
                max_sim = 0
                for sel_idx, _, _ in selected:
                    orig_sel_idx = ranked_sentences[sel_idx][0]
                    sim = cosine_similarity(
                        embeddings[orig_idx].reshape(1, -1),
                        embeddings[orig_sel_idx].reshape(1, -1)
                    )[0][0]
                    max_sim = max(max_sim, sim)
                
                mmr = self.lambda_mmr * rel_score - (1 - self.lambda_mmr) * max_sim
                mmr_scores.append((idx, mmr))
            
            best_idx = max(mmr_scores, key=lambda x: x[1])[0]
            selected.append(ranked_sentences[best_idx])
            remaining.remove(best_idx)
        
        return sorted(selected, key=lambda x: x[0])
