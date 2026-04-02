from rouge_score import rouge_scorer
from rouge_score import tokenizers
import re


class RussianTokenizer:
    def tokenize(self, text):
        text = text.lower()
        text = text.replace('—', '-').replace('–', '-')
        tokens = re.findall(r'\b\w+\b', text)
        return tokens


def read_summaries(path):
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()
    
    summaries = re.findall(r'"(.*?)"', content, flags=re.DOTALL)
    return summaries


def evaluate_rouge(generated_summaries, reference_summaries):    
    if len(generated_summaries) != len(reference_summaries):
        print(f"Предупреждение: количество рефератов не совпадает")
        print(f"  Сгенерировано: {len(generated_summaries)}")
        print(f"  Эталонных: {len(reference_summaries)}")
        return
    
    tokenizer = RussianTokenizer()
    scorer = rouge_scorer.RougeScorer(['rouge1', 'rouge2', 'rougeL'], use_stemmer=False, tokenizer=tokenizer)
    
    all_scores = {
        'rouge1': {'precision': [], 'recall': [], 'fmeasure': []},
        'rouge2': {'precision': [], 'recall': [], 'fmeasure': []},
        'rougeL': {'precision': [], 'recall': [], 'fmeasure': []}
    }
    
    print("\n" + "="*60)
    print("ОЦЕНКА ROUGE")
    print("="*60)
    
    for i, (generated, reference) in enumerate(zip(generated_summaries, reference_summaries)):
        print(f"\nТекст {i+1}:")
        print(f"  Сгенерировано: {len(generated)} символов")
        print(f"  Эталон: {len(reference)} символов")
        
        gen_words = generated.split()[:5]
        ref_words = reference.split()[:5]
        print(f"  Generated words: {' '.join(gen_words)}")
        print(f"  Reference words: {' '.join(ref_words)}")
        
        scores = scorer.score(reference, generated)
        
        for metric in ['rouge1', 'rouge2', 'rougeL']:
            all_scores[metric]['precision'].append(scores[metric].precision)
            all_scores[metric]['recall'].append(scores[metric].recall)
            all_scores[metric]['fmeasure'].append(scores[metric].fmeasure)
            
            print(f"  {metric.upper()}: P={scores[metric].precision:.3f}, R={scores[metric].recall:.3f}, F1={scores[metric].fmeasure:.3f}")
    
    print("\n")
    print("средние значения")
    
    for metric in ['rouge1', 'rouge2', 'rougeL']:
        avg_precision = sum(all_scores[metric]['precision']) / len(all_scores[metric]['precision'])
        avg_recall = sum(all_scores[metric]['recall']) / len(all_scores[metric]['recall'])
        avg_fmeasure = sum(all_scores[metric]['fmeasure']) / len(all_scores[metric]['fmeasure'])
        
        print(f"\n{metric.upper()}:")
        print(f"  Precision: {avg_precision:.3f}")
        print(f"  Recall:    {avg_recall:.3f}")
        print(f"  F1-score:  {avg_fmeasure:.3f}")
    
    final_score = sum(all_scores['rouge1']['fmeasure']) / len(all_scores['rouge1']['fmeasure'])
    print(f"итоговая оценка (ROUGE-1 F1): {final_score:.3f}")


def main():
    import sys
    import os
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    sys.path.insert(0, parent_dir)
    
    if len(sys.argv) >= 4:
        input_path = sys.argv[1]
        output_path = sys.argv[2]
        reference_path = sys.argv[3]
    else:
        input_path = os.path.join(script_dir, "input_evaluate.txt")
        output_path = os.path.join(script_dir, "output_evaluate.txt")
        reference_path = os.path.join(script_dir, "gold_standard.txt")
    
    try:
        original_texts = read_summaries(input_path)
        
        from main import summarize_text
        generated = []
        for text in original_texts:
            summary = summarize_text(text)
            generated.append(summary)
        
        with open(output_path, "w", encoding="utf-8") as f:
            f.write("[\n")
            for i, summary in enumerate(generated):
                summary_escaped = summary.replace('"', "'")
                f.write(f'  "{summary_escaped}"')
                if i != len(generated) - 1:
                    f.write(",")
                f.write("\n")
            f.write("]\n")
        
        print(f"Сохранено в {output_path}")
        reference = read_summaries(reference_path)
        
        print(f"\nФайлы:")
        print(f"  Входные тексты: {os.path.basename(input_path)}")
        print(f"  Сгенерированные: {os.path.basename(output_path)}")
        print(f"  Золотой стандарт: {os.path.basename(reference_path)}")
        
        evaluate_rouge(generated, reference)
        
    except FileNotFoundError as e:
        print(f"Ошибка: {e}")
        print("\nИспользование:")
        print(f"  python evaluate/evaluate.py [input.txt] [output.txt] [gold_standard.txt]")
        print(f"  По умолчанию: файлы из папки evaluate/")
    except Exception as e:
        print(f"Ошибка: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
