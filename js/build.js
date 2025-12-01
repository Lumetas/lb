// Объединенный файл от Пн 01 дек 2025 13:55:40 +05
// Файл: js/googleSearch.js
(function() {
    'use strict';

    // Конфигурация
    const CONFIG = {
        HOTKEY: {
            ctrl: true,
            shift: false,
            key: 'g'
        },
        SEARCH_ENGINES: {
            google: 'https://www.google.com/search?q=',
            duckduckgo: 'https://duckduckgo.com/?q=',
            bing: 'https://www.bing.com/search?q=',
            youtube: 'https://www.youtube.com/results?search_query=',
            github: 'https://github.com/search?q='
        },
        DEFAULT_ENGINE: 'google'
    };

    // Создаем и стилизуем модальное окно
    let modal = null;
    let input = null;
    let select = null;

    function createModal() {
        // Создаем элементы
        modal = document.createElement('div');
        const modalContent = document.createElement('div');
        const modalHeader = document.createElement('div');
        const title = document.createElement('h3');
        const closeBtn = document.createElement('span');
        const form = document.createElement('form');
        input = document.createElement('input');
        // select = document.createElement('select');
        const buttonGroup = document.createElement('div');
        // const goBtn = document.createElement('button');
        // const searchBtn = document.createElement('button');

        // Устанавливаем содержимое
        title.textContent = 'Быстрая навигация';
        closeBtn.innerHTML = '&times;';
        input.type = 'text';
        input.placeholder = 'Введите URL или поисковый запрос...';
        input.autofocus = true;
        // goBtn.textContent = 'Перейти';
        // searchBtn.textContent = 'Поиск';

        // Собираем структуру
        modalHeader.appendChild(title);
        modalHeader.appendChild(closeBtn);
        // buttonGroup.appendChild(goBtn);
        // buttonGroup.appendChild(searchBtn);
        // form.appendChild(select);
        form.appendChild(input);
        form.appendChild(buttonGroup);
        modalContent.appendChild(modalHeader);
        modalContent.appendChild(form);
        modal.appendChild(modalContent);

        // Добавляем на страницу
        document.body.appendChild(modal);

        // Стилизация
        modal.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.7);
            display: none;
            justify-content: center;
            align-items: center;
            z-index: 999999;
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        `;

        modalContent.style.cssText = `
            background: white;
            padding: 25px;
            border-radius: 12px;
            box-shadow: 0 10px 40px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 500px;
            animation: modalAppear 0.3s ease;
        `;

        modalHeader.style.cssText = `
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 20px;
        `;

        title.style.cssText = `
            margin: 0;
            color: #333;
            font-size: 22px;
            font-weight: 600;
        `;

        closeBtn.style.cssText = `
            cursor: pointer;
            font-size: 28px;
            color: #666;
            line-height: 1;
            user-select: none;
            transition: color 0.2s;
        `;

        closeBtn.onmouseover = () => closeBtn.style.color = '#333';
        closeBtn.onmouseout = () => closeBtn.style.color = '#666';

        form.style.cssText = `
            display: flex;
            flex-direction: column;
            gap: 15px;
        `;

        // select.style.cssText = `
        //     padding: 10px 15px;
        //     border: 2px solid #e0e0e0;
        //     border-radius: 8px;
        //     font-size: 16px;
        //     outline: none;
        //     transition: border-color 0.3s;
        //     background: white;
        //     cursor: pointer;
        // `;

        // select.onfocus = () => select.style.borderColor = '#4285f4';
        // select.onblur = () => select.style.borderColor = '#e0e0e0';

        input.style.cssText = `
            padding: 12px 15px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
            outline: none;
            transition: border-color 0.3s;
        `;

        input.onfocus = () => input.style.borderColor = '#4285f4';
        input.onblur = () => input.style.borderColor = '#e0e0e0';

		input.addEventListener('keydown', (e) => {
			if (e.key === 'Enter') {
				e.preventDefault();
				if (e.ctrlKey) {
					handleGo(e.target.value);
					return;
				}
				handleSearch(e.target.value);
			}
		});

        buttonGroup.style.cssText = `
            display: flex;
            gap: 10px;
            margin-top: 10px;
        `;

        const buttonStyle = `
            flex: 1;
            padding: 12px 20px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.3s;
            outline: none;
        `;

        // goBtn.style.cssText = buttonStyle + `
        //     background: #f0f0f0;
        //     color: #333;
        // `;

        // searchBtn.style.cssText = buttonStyle + `
        //     background: #4285f4;
        //     color: white;
        // `;

        // goBtn.onmouseover = () => goBtn.style.background = '#e0e0e0';
        // goBtn.onmouseout = () => goBtn.style.background = '#f0f0f0';
        // searchBtn.onmouseover = () => searchBtn.style.background = '#3367d6';
        // searchBtn.onmouseout = () => searchBtn.style.background = '#4285f4';

        // Анимация появления
        const style = document.createElement('style');
        style.textContent = `
            @keyframes modalAppear {
                from {
                    opacity: 0;
                    transform: translateY(-20px) scale(0.95);
                }
                to {
                    opacity: 1;
                    transform: translateY(0) scale(1);
                }
            }
        `;
        document.head.appendChild(style);

        // Обработчики событий
        closeBtn.addEventListener('click', closeModal);
        modal.addEventListener('click', (e) => {
            if (e.target === modal) closeModal();
        });


    }

    // Открыть модальное окно
    function openModal() {
        if (!modal) createModal();
        modal.style.display = 'flex';
        input.focus();
        input.select();
    }

    // Закрыть модальное окно
    function closeModal() {
        if (modal) {
            modal.style.display = 'none';
            input.value = '';
        }
    }

    // Обработка перехода по URL
    function handleGo(url) {
        const value = url.trim();
        if (!value) return;
		if (!value.startsWith('http') || !value.startsWith('https')) {
			value = "http://" + value;
		}

		window.location.href = value;
        
    }

    // Обработка поиска
    function handleSearch() {
        let query = input.value.trim();
		query = "https://www.google.com/search?q=" + encodeURI(query)
		console.log(query);
		handleGo(query);
        closeModal();
    }

    // Обработка горячих клавиш
    function handleKeyDown(e) {
		console.log(e);
        if (e.ctrlKey === CONFIG.HOTKEY.ctrl && 
            e.shiftKey === CONFIG.HOTKEY.shift && 
            e.key.toLowerCase() === CONFIG.HOTKEY.key) {
			console.log('Ctrl+Shift+G');
            e.preventDefault();
            openModal();
        }
        
        // ESC закрывает модальное окно
        if (e.key === 'Escape' && modal && modal.style.display === 'flex') {
            closeModal();
        }
    }

    // Инициализация
    function init() {
        document.addEventListener('keydown', handleKeyDown);
        
        // Создаем модальное окно заранее для быстрого открытия
        createModal();
    }

    // Запуск при загрузке
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }
})();

