from preprocessing import TextPreprocessor
from tfidf import TfidfScorer
from embeddings import EmbeddingScorer
from ranking import Ranker
from postprocessing import SummaryBuilder
import re
import traceback

def summarize_text(text):
    if not text or not text.strip():
        return ""

    try:
        preprocessor = TextPreprocessor()

        sentences, clean_sentences = preprocessor.preprocess(text)

        if not sentences:
            return text[:300]

        tfidf = TfidfScorer(preprocessor.stop_words)
        embedder = EmbeddingScorer()
        ranker = Ranker()
        builder = SummaryBuilder(max_length=300)

        tfidf_scores = tfidf.score_sentences(clean_sentences)
        embed_scores, embeddings = embedder.score_sentences(sentences)

        ranked = ranker.rank(sentences, tfidf_scores, embed_scores)
        
        mmr_ranked = ranker.apply_mmr(ranked, embeddings)

        return builder.build(mmr_ranked)
    except Exception as e:
        print(f"Ошибка при обработке текста: {e}")
        traceback.print_exc()
        return text[:300]


def read_input_file(path):
    try:
        with open(path, "r", encoding="utf-8") as f:
            content = f.read()
    except FileNotFoundError:
        raise FileNotFoundError(f"файл {path} не найден")
    except PermissionError:
        raise PermissionError(f"нет прав на чтение {path}")
    except Exception as e:
        raise Exception(f"ошибка чтения файла {path}: {e}")

    texts = re.findall(r'"(.*?)"', content, flags=re.DOTALL)

    if not texts:
        raise ValueError(f"в файле {path} не найдено текстов в кавычках")

    return texts


def write_output_file(path, summaries):
    try:
        with open(path, "w", encoding="utf-8") as f:
            f.write("[\n")
            for i, summary in enumerate(summaries):
                summary = summary.replace('"', "'")

                f.write(f'  "{summary}"')
                if i != len(summaries) - 1:
                    f.write(",")
                f.write("\n")
            f.write("]\n")
    except PermissionError:
        raise PermissionError(f"нет прав на запись в {path}")
    except Exception as e:
        raise Exception(f"ошибка записи в файл {path}: {e}")


def main():
    input_path = "input.txt"
    output_path = "output.txt"

    try:
        texts = read_input_file(input_path)
        summaries = []
        
        for text in texts:
            summary = summarize_text(text)
            summaries.append(summary)

        write_output_file(output_path, summaries)

    except (FileNotFoundError, ValueError, PermissionError) as e:
        print(f"{e}")
    except Exception as e:
        print(f"ошибка: {e}")
        traceback.print_exc()


if __name__ == "__main__":
    main()