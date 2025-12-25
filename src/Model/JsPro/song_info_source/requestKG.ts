import https from 'https'

// 酷狗音乐搜索实现 - 歌曲搜索API
async function searchMusic(keyword: string, page: number = 1, limit: number = 20) {
  return new Promise((resolve, reject) => {
    // 使用酷狗的歌曲搜索API
    const url = `/api/v3/search/song?keyword=${encodeURIComponent(keyword)}&page=${page}&pagesize=${limit}&showtype=10&plat=2&version=7910&tag=1&correct=1&privilege=1&sver=5`

    const options = {
      hostname: 'ioscdn.kugou.com',
      path: url,
      method: 'GET',
      rejectUnauthorized: false,
      headers: {
        'User-Agent':
          'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
        Referer: 'https://kugou.com',
      },
    }

    const req = https.request(options, (res) => {
      let data = ''

      res.on('data', (chunk) => {
        data += chunk
      })

      res.on('end', () => {
        try {
          const response = JSON.parse(data)
          resolve(response)
        } catch (error) {
          reject(error)
        }
      })
    })

    req.on('error', (error) => {
      reject(error)
    })

    req.end()
  })
}

// 处理搜索结果
function handleSearchResult(response: any) {
  if (response.status !== 1) {
    throw new Error(`搜索失败: ${response.error_msg || 'API返回错误'}`)
  }

  const data = response.data || {}
  const songs = data.info || []
  
  if (!Array.isArray(songs)) {
    throw new Error('搜索结果格式错误')
  }

  // 处理歌曲信息
  const list = songs
    .map((item: any) => ({
      name: item.songname || '',
      singer: item.singername || '未知艺术家',
      album: item.album_name || '未知专辑',
      interval: formatTime(item.duration || 0),
      duration: item.duration || 0,
      source: 'kg',
    }))

  return {
    total: data.total || 0,
    list,
  }
}

// 格式化时间（秒转 mm:ss）
function formatTime(seconds: number): string {
  const mins = Math.floor(seconds / 60)
  const secs = seconds % 60
  return `${mins}:${secs.toString().padStart(2, '0')}`
}

// 主函数
async function main() {
  try {
    console.log('🔍 正在通过酷狗搜索 "成都"...\n')

    const response = await searchMusic('成都', 1, 10)
    const result = handleSearchResult(response)

    console.log(`✅ 搜索完成，共找到 ${result.total} 首歌曲\n`)
    console.log('📋 前 10 首结果:\n')
    console.log('─'.repeat(80))

    result.list.forEach((song: any, index: number) => {
      console.log(`${index + 1}. 【${song.name}】`)
      console.log(`   歌手: ${song.singer}`)
      console.log(`   专辑: ${song.album}`)
      console.log(`   时长: ${song.interval}`)
      console.log('─'.repeat(80))
    })
  } catch (error) {
    console.error('❌ 搜索出错:', error)
  }
}

main()
